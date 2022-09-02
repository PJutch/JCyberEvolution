/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "FieldView.h"
#include "utility.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_sugar.hpp>
#include <dirent.h>
#include <ImGuiFileDialog.h>

#include <SFML/Graphics.hpp>
using sf::FloatRect;
using sf::Vector2f;
using sf::Vector2i;
using sf::Color;
using sf::RenderTarget;
using sf::RenderStates;
using sf::RectangleShape;
using sf::View;
using sf::Transform;
using sf::Triangles;

#include <SFML/System.hpp>
using sf::Keyboard;
using sf::Event;
using sf::Time;

#include <algorithm>
using std::clamp;
using std::ranges::copy;
using std::ranges::fill;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <array>
using std::array;

#include <string>
using std::string;

using std::ssize;

#include <memory>
using std::make_unique;
using std::unique_ptr;

#include <algorithm>
using std::max;
using std::min;

#include <cmath>
using std::pow;
using std::fmod;
using std::fmodf;

const int POPULATION_HISTORY_SIZE = 128;

FieldView::FieldView(Vector2f screenSize, uint64_t seed) : 
        m_field{nullptr}, m_fieldWidth{128}, m_fieldHeight{128}, m_fieldTopology{Field::Topology::TORUS},
        m_randomEngine{seed}, m_cellsVertices{Triangles}, m_botsVertices{Triangles}, m_view{},
        m_screenSize{screenSize}, m_zoom{1.0f}, m_shouldDrawBots{true}, 
        m_fillDensity{0.5f}, m_simulationSpeed{1.f}, m_simulationStepRest{0.f}, m_paused{false}, 
        m_tool{Tool::SELECT_BOT}, m_selectedBot{-1, -1}, m_selectionShape{{0.f, 0.f}},
        m_recentFiles{}, m_selectedFile{-1}, m_loadedBot{nullptr}, 
        m_populationHistory(128, 0),
        m_baseZoomingChange{1.1f}, m_baseMovingSpeed{10.f}, m_speedModificator{10.f} {
    m_selectionShape.setFillColor(Color::Transparent);
    m_selectionShape.setOutlineColor(Color::Red);
    m_selectionShape.setOutlineThickness(0.25);
    m_selectionShape.setOrigin(0.25, 0.25);

    resize(screenSize.x, screenSize.y);
}

bool FieldView::handleMouseWheelScrollEvent(const Event::MouseWheelScrollEvent& event) noexcept {
    if (!m_field) return false;

    m_view.zoom(1 / m_zoom);

    float zoomChange = -event.delta;
    if (Keyboard::isKeyPressed(Keyboard::LShift)) zoomChange *= m_speedModificator;
    if (Keyboard::isKeyPressed(Keyboard::LControl)) zoomChange /= m_speedModificator;

    m_zoom *= pow(m_baseZoomingChange, zoomChange);
    m_zoom = clamp(m_zoom, 0.01f, 1.f);

    m_view.zoom(m_zoom);

    return true;
}

bool FieldView::handleMouseButtonPressedEvent(const Event::MouseButtonEvent& event, 
                                              const RenderTarget& target) noexcept {
    if (!m_field) return false;

    if (!m_view.getViewport().contains(static_cast<float>(event.x) / target.getSize().x, 
            static_cast<float>(event.y) / target.getSize().y)) {
        if (m_tool == Tool::SELECT_BOT) {
            selectBot({-1, -1});
            return true;
        }
        return false;
    }

    Vector2f posf = target.mapPixelToCoords({event.x, event.y}, m_view);
    Vector2i pos(posf.x, posf.y);
    if (!m_field->makeIndicesSafe(pos.x, pos.y)) return handleOutsideClick();

    switch (m_tool) {
        case Tool::SELECT_BOT:
            if (!m_field->at(pos.x, pos.y).hasBot()) {
                selectBot({-1, -1});
            } else selectBot(Vector2i(pos.x, pos.y));
            return true;
        case Tool::DELETE_BOT:
            m_field->at(pos.x, pos.y).deleteBot();
            return true;
        case Tool::PLACE_BOT:
            if (!m_loadedBot) {
                if (m_selectedFile == -1) {
                    m_field->at(pos.x, pos.y).setBot(
                        make_unique<Bot>(Bot::createRandom(m_field->getRandomEngine())));
                    return true;
                }

                ifstream file{m_recentFiles[m_selectedFile].second};

                m_loadedBot = make_unique<Bot>();
                file >> *m_loadedBot;
            }

            m_field->at(pos.x, pos.y).setBot(make_unique<Bot>(*m_loadedBot));
            return true;
    }

    return false;
}

void FieldView::updateField() noexcept {
    if (!m_field) return;

    m_simulationStepRest += getSimulationSpeed();
    while (m_simulationStepRest >= 1.f) {
        m_field->update();
        m_populationHistory.pop_front();
        m_populationHistory.push_back(m_field->computePopulation());

        -- m_simulationStepRest;
    }
}

void FieldView::update(bool keyboardAvailable, Time elapsedTime) noexcept {
    if (!m_field) return;

    if (keyboardAvailable) {
        float moved = m_baseMovingSpeed * elapsedTime.asSeconds();
        if (Keyboard::isKeyPressed(Keyboard::LShift)) moved *= m_speedModificator;
        if (Keyboard::isKeyPressed(Keyboard::LControl)) moved /= m_speedModificator;

        if (Keyboard::isKeyPressed(Keyboard::W))
            m_view.setCenter(m_view.getCenter().x, m_view.getCenter().y - moved); 
        if (Keyboard::isKeyPressed(Keyboard::S))
            m_view.setCenter(m_view.getCenter().x, m_view.getCenter().y + moved); 
        if (Keyboard::isKeyPressed(Keyboard::A))
            m_view.setCenter(m_view.getCenter().x - moved, m_view.getCenter().y); 
        if (Keyboard::isKeyPressed(Keyboard::D))
            m_view.setCenter(m_view.getCenter().x + moved, m_view.getCenter().y); 
    }

    for (int x = 0; x < m_field->getWidth(); ++ x)
        for (int y = 0; y < m_field->getHeight(); ++ y) {
            int offset = y * m_field->getWidth() * 6 + x * 6;
            for (int i = 0; i < 6; ++ i) {
                if (0.25f * getScreenToViewRatio() >= 1.f) {
                    m_cellsVertices[offset + i].color = m_field->at(x, y).getColor();
                    if (m_field->at(x, y).hasBot()) {
                        m_botsVertices[offset + i].color = m_field->at(x, y).getBot().getColor();
                    } else {
                        m_botsVertices[offset + i].color = Color::Transparent;
                    }
                } else {
                    if (m_field->at(x, y).hasBot()) {
                        if (m_selectedBot == Vector2i{x, y}) {
                            m_cellsVertices[offset + i].color = Color::Red;
                        } else {
                            m_cellsVertices[offset + i].color 
                                = m_field->at(x, y).getBot().getColor();
                        }
                    } else {
                        m_cellsVertices[offset + i].color =m_field->at(x, y).getColor();
                    }
                }
            }
    }
}

void FieldView::drawField(RenderTarget& target, RenderStates states) const noexcept {
    target.draw(m_cellsVertices, states);
    if (0.25f * getScreenToViewRatio() >= 1.f) {
        target.draw(m_botsVertices, states);

        for (Cell& cell : *m_field) 
            if (cell.hasBot()) cell.getBot().drawDirection(target, states);

        if (m_selectedBot != Vector2i(-1, -1)) 
            target.draw(m_selectionShape, states);
    }
}

void FieldView::drawCone(RenderTarget& target, RenderStates states, Vector2f apex) const noexcept {
    for (float rotation = 0.f; rotation < 360.f; rotation += 90.f) {
        RenderStates currentStates = states;
        currentStates.transform.rotate(rotation, apex);
        drawField(target, currentStates);
    }

    RectangleShape fieldBorderShape;
    fieldBorderShape.setFillColor(Color::Transparent);
    fieldBorderShape.setOutlineColor(Color::Black);
    fieldBorderShape.setOutlineThickness(1.f);
    fieldBorderShape.setSize({2.f * m_field->getSize()});
    fieldBorderShape.setOrigin(m_field->getSize());
    fieldBorderShape.setPosition(apex);
    target.draw(fieldBorderShape, states);
}

void FieldView::draw(RenderTarget& target, RenderStates states) const noexcept {
    if (!m_field) return;

    View prevView = target.getView();

    RectangleShape fieldBorderShape;
    fieldBorderShape.setFillColor(Color::Transparent);
    fieldBorderShape.setOutlineColor(Color::Black);
    fieldBorderShape.setOutlineThickness(1.f);

    RectangleShape borderShape;
    borderShape.setSize({prevView.getSize().x * m_view.getViewport().width,  
                        prevView.getSize().y * m_view.getViewport().height});
    borderShape.setPosition(prevView.getSize().x * m_view.getViewport().left,  
                            prevView.getSize().y * m_view.getViewport().top);
    borderShape.setFillColor(Color::Transparent);
    borderShape.setOutlineColor(Color::Black);
    borderShape.setOutlineThickness(5.f);
    target.draw(borderShape, states);

    target.setView(m_view);

    Vector2f viewStart = m_view.getCenter() - m_view.getSize() / 2.f;
    Vector2f viewEnd   = m_view.getCenter() + m_view.getSize() / 2.f;

    switch (m_field->getTopology()) {
    case Field::Topology::TORUS: {
            Vector2f renderStart{
                getFirstInInterval(0.f, m_field->getWidth(), 
                                   viewStart.x, viewEnd.x),
                getFirstInInterval(0.f, m_field->getHeight(), 
                                   viewStart.y, viewEnd.y)};

            for (float y = renderStart.y; y < viewEnd.y; y += m_field->getHeight())
                for (float x = renderStart.x; x < viewEnd.x; x += m_field->getWidth()) {
                    RenderStates currentStates = states;
                    currentStates.transform.translate(x, y);
                    drawField(target, currentStates);
            }
            break;
        }
    case Field::Topology::CYLINDER_Y: {
            float renderStart = getFirstInInterval(0.f, m_field->getHeight(), 
                viewStart.y, viewEnd.y);

            for (float y = renderStart; y < viewEnd.y; y += m_field->getHeight()) {
                RenderStates currentStates = states;
                currentStates.transform.translate(0, y);
                drawField(target, currentStates);
            }
            
            fieldBorderShape.setSize({m_field->getSize().x, m_view.getSize().y});
            fieldBorderShape.setPosition(0.f,
                                         m_view.getCenter().y - m_view.getSize().y / 2);
            target.draw(fieldBorderShape, states);
            break;
        }
    case Field::Topology::CYLINDER_X: {
            float renderStart = getFirstInInterval(0.f, m_field->getHeight(), 
                viewStart.x, viewEnd.x);

            for (float x = renderStart; x < viewEnd.x; x += m_field->getHeight()) {
                RenderStates currentStates = states;
                currentStates.transform.translate(x, 0);
                drawField(target, currentStates);
            }

            fieldBorderShape.setSize({m_view.getSize().x, m_field->getSize().y});
            fieldBorderShape.setPosition(m_view.getCenter().x - m_view.getSize().x / 2, 
                                         0.f);
            target.draw(fieldBorderShape, states);
            break;
        }
    case Field::Topology::PLANE:
        drawField(target, states);
        fieldBorderShape.setSize(m_field->getSize());
        fieldBorderShape.setPosition(0.f, 0.f);
        target.draw(fieldBorderShape, states);
        break;
    case Field::Topology::SPHERE_LEFT: {
            Vector2f renderStart{
                getFirstInInterval(0.f, 2 * m_field->getWidth(), 
                                   viewStart.x, viewEnd.x),
                getFirstInInterval(0.f, 2 * m_field->getHeight(), 
                                   viewStart.y, viewEnd.y)};

            for (float y = renderStart.y; y < viewEnd.y; y += 2 * m_field->getHeight())
                for (float x = renderStart.x; x < viewEnd.x; x += 2 * m_field->getWidth()) {
                    RenderStates translatedStates = states;
                    translatedStates.transform.translate(x, y);

                    for (float rotation = 0.f; rotation < 360.f; rotation += 90.f) {
                        RenderStates currentStates = translatedStates;
                        currentStates.transform.rotate(rotation, m_field->getSize());
                        drawField(target, currentStates);
                    }
            }
            break;
        }
    case Field::Topology::SPHERE_RIGHT: {
            Vector2f renderStart{
                getFirstInInterval(0.f, 2 * m_field->getWidth(), 
                                   viewStart.x, viewEnd.x),
                getFirstInInterval(0.f, 2 * m_field->getHeight(), 
                                   viewStart.y, viewEnd.y)};

            for (float y = renderStart.y; y < viewEnd.y; y += 2 * m_field->getHeight())
                for (float x = renderStart.x; x < viewEnd.x; x += 2 * m_field->getWidth()) {
                    RenderStates translatedStates = states;
                    translatedStates.transform.translate(x, y);
                    drawField(target, translatedStates);

                    RenderStates currentStates = translatedStates;
                    currentStates.transform.rotate(-90.f, 
                        0.f + m_field->getWidth(), 0.f);
                    drawField(target, currentStates);

                    currentStates = translatedStates;
                    currentStates.transform.rotate(180.f, 
                        0.f + m_field->getWidth(), 
                        0.f + m_field->getHeight());
                    drawField(target, currentStates);

                    currentStates = translatedStates;
                    currentStates.transform.rotate(90.f, 0.f, 
                        0.f + m_field->getHeight());
                    drawField(target, currentStates);
            }
            break;
        }
    case Field::Topology::CONE_LEFT_TOP:
        drawCone(target, states, Vector2f(0.f, 0.f));
        break;
    case Field::Topology::CONE_RIGHT_TOP:
        drawCone(target, states, Vector2f(m_field->getWidth(), 0));
        break;
    case Field::Topology::CONE_LEFT_BOTTOM:
        drawCone(target, states, Vector2f(0, m_field->getHeight()));
        break;
    case Field::Topology::CONE_RIGHT_BOTTOM:
        drawCone(target, states, m_field->getSize());
        break;
    }

    target.setView(prevView);
}

void FieldView::showSelectBotTypeGui() noexcept {
    ImGui::BeginDisabled(m_tool != Tool::PLACE_BOT);
    ImGui::Text("Select bot type");
    with_ListBox ("##Select bot type", 
            ImVec2(-FLT_MIN, 5.25f * ImGui::GetTextLineHeightWithSpacing())) {
        const bool is_selected = (m_selectedFile == -1);
        if (ImGui::Selectable("Random", is_selected)) {
            m_selectedFile = -1;
            m_loadedBot.reset();
        }
            
        if (is_selected)
            ImGui::SetItemDefaultFocus();

        for (int i = 0; i < ssize(m_recentFiles); i++) {
            with_ID (i) {
                const bool is_selected = (m_selectedFile == i);
                if (ImGui::Selectable(m_recentFiles[i].first.c_str(), is_selected)) {
                    m_selectedFile = i;
                    m_loadedBot.reset();
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
        }
    }
    if (ImGui::Button("Other...")) {
        ImGuiFileDialog::Instance()->OpenDialog("Open bot", "Choose File", 
            ".bot", ".", "", 1, nullptr, ImGuiFileDialogFlags_None);
    }
    ImGui::EndDisabled();

    if (ImGuiFileDialog::Instance()->Display("Open bot")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            for (auto [name, path] : ImGuiFileDialog::Instance()->GetSelection()) {
                if (ssize(m_recentFiles) >= 4) m_recentFiles.pop_back();
                m_recentFiles.emplace_front(name, path);
                m_selectedFile = 0;
                m_loadedBot.reset();
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }  
}

void FieldView::showSaveBotGui() noexcept {
    ImGui::BeginDisabled(m_selectedBot == Vector2i(-1, -1));
    if (ImGui::Button("Save selected bot")) {
        ImGuiFileDialog::Instance()->OpenDialog("Save bot", "Choose File", 
            ".bot", ".", "", 1, nullptr, ImGuiFileDialogFlags_ConfirmOverwrite);
    }
    ImGui::EndDisabled();

    if (ImGuiFileDialog::Instance()->Display("Save bot")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            if (m_selectedBot != Vector2i(-1, -1)) {
                ofstream file{ImGuiFileDialog::Instance()->GetFilePathName()};

                Cell& cell = m_field->at(m_selectedBot.x, m_selectedBot.y);
                file << cell.getBot() << std::endl;
            }
        }

        ImGuiFileDialog::Instance()->Close();
    }  
}

void FieldView::showTopologyCombo() noexcept {
    int topology = static_cast<int>(m_field->getTopology());

    if (m_field->getWidth() == m_field->getHeight()) {
        ImGui::Combo("Topology", &topology, "Torus\0Cylinder X\0Cylinder Y\0Plane\0"
                                            "Sphere left\0Sphere right\0Cone left top\0"
                                            "Cone right top\0Cone left bottom\0"
                                            "Cone right bottom\0");
    } else {
        ImGui::Combo("Topology", &topology, "Torus\0Cylinder X\0Cylinder Y\0Plane\0");
    }
    m_field->setTopology(static_cast<Field::Topology>(topology));
}

void FieldView::showNewFieldTopologyCombo() noexcept {
    int topology = static_cast<int>(m_fieldTopology);
    
    if (m_fieldWidth == m_fieldHeight) {
        ImGui::Combo("Topology", &topology, "Torus\0Cylinder X\0Cylinder Y\0Plane\0"
                                            "Sphere left\0Sphere right\0Cone left top\0"
                                            "Cone right top\0Cone left bottom\0"
                                            "Cone right bottom\0");
    } else {
        if (topology > 3) topology = 0;
        ImGui::Combo("Topology", &topology, "Torus\0Cylinder X\0Cylinder Y\0Plane\0");
    }
    m_fieldTopology = static_cast<Field::Topology>(topology);
}

void FieldView::showGui() noexcept {
    if (m_field) {
        with_Window("View") {
            if (ImGui::Button("To center")) {
                m_view.setCenter(m_field->getSize() / 2.f);
            }
            ImGui::SliderFloat("Simulation speed", &m_simulationSpeed, 0.f, 16.f);
        }

        with_Window("Tools") {
            ImGui::SliderFloat("Fill density", &m_fillDensity, 0.f, 1.f);
            if (ImGui::Button("Random fill")) {
                m_selectedBot = {-1, -1};
                m_field->randomFill(m_fillDensity);
                fill(m_populationHistory, m_field->computePopulation());
            }

            if (ImGui::Button("Clear")) {
                m_selectedBot = {-1, -1};
                m_field->clear();
                fill(m_populationHistory, 0);
            }

            int tool = static_cast<int>(m_tool);
            ImGui::Combo("Click tool", &tool, "Select bot\0Delete bot\0Place bot\0");
            m_tool = static_cast<Tool>(tool);
            if (m_tool != Tool::SELECT_BOT) m_selectedBot = {-1, -1};

            showSelectBotTypeGui();
            showSaveBotGui();
        }

        with_Window("Statistics") {
            ImGui::Text("Epoch: %i", m_field->getEpoch());

            ImGui::Text("Population: %i", m_populationHistory.back());
            ImGui::PlotLines("##Population", containerGetter<std::deque<int>>, &m_populationHistory, 
                            POPULATION_HISTORY_SIZE, 0, NULL, 
                            0.f, m_field->getWidth() * m_field->getHeight(), ImVec2(0, 80.0f));
        }

        with_Window("Life cycle") {
            int lifetime = m_field->getLifetime();
            if (ImGui::SliderInt("Lifetime", &lifetime, 0, 1024)) {
                m_field->setLifetime(lifetime);
            }

            float mutationChance = m_field->getMutationChance();
            if (ImGui::SliderFloat("Mutation chance", &mutationChance, 0, 1, 
                                "%.3f", ImGuiSliderFlags_Logarithmic)) {
                m_field->setMutationChance(mutationChance);
            }
        }

        with_Window("Field") {
            showTopologyCombo();
            if (ImGui::Button("New")) m_field.reset();
        }
    } else {
        with_Window("New field") {
            ImGui::SliderInt("Width", &m_fieldWidth, 16, 1024);
            ImGui::SliderInt("Height", &m_fieldHeight, 16, 1024);
            showNewFieldTopologyCombo();
            if (ImGui::Button("Create")) {
                setField(make_unique<Field>(m_fieldWidth, m_fieldHeight, m_randomEngine()));
                m_field->setTopology(m_fieldTopology);

                m_cellsVertices.resize(m_field->getWidth() * m_field->getHeight() * 6);
                m_botsVertices.resize(m_field->getWidth() * m_field->getHeight() * 6);
                for (int x = 0; x < m_field->getWidth(); ++ x)
                    for (int y = 0; y < m_field->getHeight(); ++ y) {
                        int offset = y * m_field->getWidth() * 6 + x * 6;

                        m_cellsVertices[offset].position = Vector2f(x, y);
                        m_cellsVertices[offset + 1].position = Vector2f(x + 1, y);
                        m_cellsVertices[offset + 2].position = Vector2f(x, y + 1);
                        m_cellsVertices[offset + 3].position = Vector2f(x + 1, y + 1);
                        m_cellsVertices[offset + 4].position = Vector2f(x + 1, y);
                        m_cellsVertices[offset + 5].position = Vector2f(x, y + 1);

                        m_botsVertices[offset].position = Vector2f(x + 0.1f, y + 0.1f);
                        m_botsVertices[offset + 1].position = Vector2f(x + 0.9f, y + 0.1f);
                        m_botsVertices[offset + 2].position = Vector2f(x + 0.1f, y + 0.9f);
                        m_botsVertices[offset + 3].position = Vector2f(x + 0.9f, y + 0.9f);
                        m_botsVertices[offset + 4].position = Vector2f(x + 0.9f, y + 0.1f);
                        m_botsVertices[offset + 5].position = Vector2f(x + 0.1f, y + 0.9f);
                }
            }
        }
        return;
    }
}
