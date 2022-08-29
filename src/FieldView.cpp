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
using sf::Transform;

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

using std::ssize;

#include <memory>
using std::make_unique;

#include <cmath>
using std::pow;
using std::fmod;
using std::fmodf;

const int POPULATION_HISTORY_SIZE = 128;

FieldView::FieldView(Vector2f screenSize, Field& field) : 
        m_field{field}, m_view{FloatRect(0.f, 0.f, field.getWidth(), field.getHeight())}, 
        m_zoom{1.0f}, m_shouldDrawBots{true}, 
        m_fillDensity{0.5f}, m_simulationSpeed{1}, m_paused{false}, 
        m_tool{Tool::SELECT_BOT}, m_selectedBot{-1, -1}, m_selectionShape{{0.f, 0.f}},
        m_recentFiles{}, m_selectedFile{-1}, m_loadedBot{nullptr}, 
        m_populationHistory(128, field.computePopulation()),
        m_baseZoomingChange{1.1f}, m_baseMovingSpeed{10.f}, m_speedModificator{10.f} {
    m_selectionShape.setFillColor(Color::Transparent);
    m_selectionShape.setOutlineColor(Color::Red);
    m_selectionShape.setOutlineThickness(0.25);
    m_selectionShape.setOrigin(0.25, 0.25);

    m_field.setView(this);
    
    resize(screenSize.x, screenSize.y);
}

bool FieldView::handleMouseWheelScrollEvent(const Event::MouseWheelScrollEvent& event) noexcept {
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
    if (!m_view.getViewport().contains(static_cast<float>(event.x) / target.getSize().x, 
            static_cast<float>(event.y) / target.getSize().y)) {
        if (m_tool == Tool::SELECT_BOT) {
            selectBot({-1, -1});
            return true;
        }
        return false;
    }

    Vector2f pos = target.mapPixelToCoords({event.x, event.y}, m_view);
    if (m_field.getTopology() == Field::Topology::TORUS) {
        pos = {fmodf(pos.x, m_field.getWidth()), fmodf(pos.y, m_field.getHeight())};
        if (pos.x < 0) pos.x += m_field.getWidth();
        if (pos.y < 0) pos.y += m_field.getHeight();
    } else if (!m_field.getRect().contains(pos)) {
        if (m_tool == Tool::SELECT_BOT) {
            selectBot({-1, -1});
            return true;
        }
        return false;
    }

    switch (m_tool) {
        case Tool::SELECT_BOT:
            if (!m_field.at(pos.x, pos.y).hasBot()) {
                selectBot({-1, -1});
            } else selectBot(Vector2i(pos.x, pos.y));
            return true;
        case Tool::DELETE_BOT:
            m_field.at(pos.x, pos.y).deleteBot();
            return true;
        case Tool::PLACE_BOT:
            if (!m_loadedBot) {
                if (m_selectedFile == -1) {
                    m_field.at(pos.x, pos.y).setBot(
                        make_unique<Bot>(Bot::createRandom(m_field.getRandomEngine())));
                    return true;
                }

                ifstream file{ImGuiFileDialog::Instance()->GetFilePathName()};

                m_loadedBot = make_unique<Bot>();
                file >> *m_loadedBot;
            }

            m_field.at(pos.x, pos.y).setBot(make_unique<Bot>(*m_loadedBot));
            return true;
    }

    return false;
}

void FieldView::resize(float width, float height) noexcept {
    if (width > height)  {
        m_view.setViewport({0.f, 0.f, height / width, 1.f});
    } else {
        m_view.setViewport({0.f, 0.f, 1.f, width / height});
    }
}

void FieldView::updateField() noexcept {
    for (int i = 0; i < getSimulationSpeed(); ++ i) {
        m_field.update();
        m_populationHistory.pop_front();
        m_populationHistory.push_back(m_field.computePopulation());
    }
}

void FieldView::update(bool keyboardAvailable, Time elapsedTime) noexcept {
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
}

void FieldView::draw(RenderTarget& target, RenderStates states) const noexcept {
    sf::View prevView = target.getView();

    sf::RectangleShape borderShape;
    borderShape.setSize({prevView.getSize().x * m_view.getViewport().width,  
                        prevView.getSize().y * m_view.getViewport().height});
    borderShape.setPosition(prevView.getSize().x * m_view.getViewport().left,  
                            prevView.getSize().y * m_view.getViewport().top);
    borderShape.setFillColor(Color::Transparent);
    borderShape.setOutlineColor(Color::Black);
    borderShape.setOutlineThickness(5.f);
    target.draw(borderShape, states);

    target.setView(m_view);

    switch (m_field.getTopology()) {
    case Field::Topology::TORUS: {
            Vector2f viewStart = m_view.getCenter() - m_view.getSize() / 2.f;
            Vector2f viewEnd = m_view.getCenter() + m_view.getSize() / 2.f;

            Vector2f relativeFieldPos = m_field.getPosition() - viewStart;
            Vector2f renderOffset{fmodf(relativeFieldPos.x, m_field.getWidth()), 
                                fmodf(relativeFieldPos.y, m_field.getHeight())};
        

            Vector2f renderStart = viewStart + renderOffset - m_field.getSize();

            for (float y = renderStart.y; y < viewEnd.y; y += m_field.getHeight()) {
                for (float x = renderStart.x; x < viewEnd.x; x += m_field.getWidth()) {
                    RenderStates currentStates = states;
                    currentStates.transform.translate(x, y);
                    currentStates.transform.translate(-m_field.getPosition());

                    target.draw(m_field, currentStates);

                    currentStates.transform *= m_field.getTransform();
                    if (m_selectedBot != Vector2i(-1, -1)) target.draw(m_selectionShape, currentStates);
                }
            }
            break;
        }
    case Field::Topology::PLANE: {
            target.draw(m_field, states);

            states.transform *= m_field.getTransform();
            if (m_selectedBot != Vector2i(-1, -1)) target.draw(m_selectionShape, states);
            break;
        }
    }

    target.setView(prevView);
}

void FieldView::showGui() noexcept {
    with_Window("View") {
        if (ImGui::Checkbox("Show bots", &m_shouldDrawBots)) {
            for (Cell& cell : m_field) cell.setShouldDrawBot(m_shouldDrawBots);
        }
        if (ImGui::Button("To center")) {
            m_view.setCenter(m_field.getPosition() + m_field.getSize() / 2.f);
        }
        ImGui::SliderInt("Simulation speed", &m_simulationSpeed, 0, 64);
    }

    with_Window("Tools") {
        ImGui::SliderFloat("Fill density", &m_fillDensity, 0.f, 1.f);
        if (ImGui::Button("Random fill")) {
            m_selectedBot = {-1, -1};
            m_field.randomFill(m_fillDensity);
            fill(m_populationHistory, m_field.computePopulation());
        }

        if (ImGui::Button("Clear")) {
            m_selectedBot = {-1, -1};
            m_field.clear();
            fill(m_populationHistory, 0);
        }

        ImGui::Text("Mouse");
        int tool = static_cast<int>(m_tool);
        ImGui::Combo("##Mouse", &tool, "Select bot\0Delete bot\0Place bot\0");
        m_tool = static_cast<Tool>(tool);
        if (m_tool != Tool::SELECT_BOT) m_selectedBot = {-1, -1};

        ImGui::BeginDisabled(m_tool != Tool::PLACE_BOT);
        ImGui::Text("Select bot");
        with_ListBox ("##Select bot", ImVec2(-FLT_MIN, 5.25f * ImGui::GetTextLineHeightWithSpacing())) {
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

                    Cell& cell = m_field.at(m_selectedBot.x, m_selectedBot.y);
                    file << cell.getBot() << std::endl;
                }
            }

            ImGuiFileDialog::Instance()->Close();
        }  
    }

    with_Window("Statistics") {
        ImGui::Text("Epoch: %i", m_field.getEpoch());

        ImGui::Text("Population: %i", m_populationHistory.back());
        ImGui::PlotLines("##Population", containerGetter<std::deque<int>>, &m_populationHistory, 
                         POPULATION_HISTORY_SIZE, 0, NULL, 
                         0.f, m_field.getWidth() * m_field.getHeight(), ImVec2(0, 80.0f));
    }

    with_Window("Life cycle") {
        int lifetime = m_field.getLifetime();
        if (ImGui::SliderInt("Lifetime", &lifetime, 0, 1024)) {
            m_field.setLifetime(lifetime);
        }

        float mutationChance = m_field.getMutationChance();
        if (ImGui::SliderFloat("Mutation chance", &mutationChance, 0, 1, 
                               "%.3f", ImGuiSliderFlags_Logarithmic)) {
            m_field.setMutationChance(mutationChance);
        }
    }

    with_Window("Field") {
        ImGui::Text("Topology");
        int topology = static_cast<int>(m_field.getTopology());
        if (ImGui::Combo("##Topology", &topology, "Torus\0Plane\0\0")) {
            m_field.setTopology(static_cast<Field::Topology>(topology));
        }
    }
}
