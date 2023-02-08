/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#include "Topology.h"

#include "Field.h"

#include <imgui.h>
#include <imgui-SFML.h>
#include <imgui_sugar.hpp>
#include <dirent.h>

#include <SFML/Graphics.hpp>

#include <utility>
#include <cassert>

using std::swap;

class TorusTopology : public Topology {
public:
    using Topology::Topology;

    Topology::Id getId() const override {
        return Topology::Id::TORUS;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        x %= m_width;
        if (x < 0) x += m_width;

        y %= m_height;
        if (y < 0) y += m_height;
        return true;
    }
};

class CylinderXTopology : public Topology {
public:
    using Topology::Topology;

    Topology::Id getId() const override {
        return Topology::Id::CYLINDER_X;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        x %= m_width;
        if (x < 0) x += m_width;
        return 0 <= y && y < m_height;
    }
};

class CylinderYTopology : public Topology {
public:
    using Topology::Topology;

    Topology::Id getId() const override {
        return Topology::Id::CYLINDER_Y;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        y %= m_height;
        if (y < 0) y += m_height;
        return 0 <= x && x < m_width;
    }
};

class PlaneTopology : public Topology {
public:
    using Topology::Topology;
    
    Topology::Id getId() const override {
        return Topology::Id::PLANE;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        return sf::IntRect(0, 0, m_width, m_height).contains(x, y);
    }
};

class SphereLeftTopology : public Topology {
public:
    SphereLeftTopology(int size) : Topology{size, size} {}

    Topology::Id getId() const override {
        return Topology::Id::SPHERE_LEFT;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        x %= 2 * m_width;
        if (x < 0) x += 2 * m_width;

        y %= 2 * m_height;
        if (y < 0) y += 2 * m_height;

        if (x < m_width) {
            if (y >= m_height) {
                swap(x, y);
                x = 2 * m_width - x - 1;

                if (rotation) {
                    rotation += 6;
                    rotation %= 8;
                }
            }
        } else {
            if (y < m_height) {
                swap(x, y);
                y = 2 * m_height - y - 1;

                if (rotation) {    
                    rotation += 2;
                    rotation %= 8;
                }
            } else {
                x = 2 * m_width - x - 1;
                y = 2 * m_height - y - 1;

                if (rotation) {    
                    rotation += 4;
                    rotation %= 8;
                }
            }
        }
        return true;
    }
};

class SphereRightTopology : public Topology {
public:
    SphereRightTopology(int size) noexcept : Topology{size, size} {}

    Topology::Id getId() const override {
        return Topology::Id::SPHERE_RIGHT;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        x %= 2 * m_width;
        if (x < 0) x += 2 * m_width;

        y %= 2 * m_height;
        if (y < 0) y += 2 * m_height;

        if (x < m_width) {
            if (y >= m_height) {
                swap(x, y);
                x = x - m_height;
                y = m_height - y - 1;

                if (rotation) {   
                    rotation += 2;
                    rotation %= 8;
                }
            }
        } else {
            if (y < m_height) {
                swap(x, y);
                x = m_width - x - 1;
                y = y - m_width;

                if (rotation) {   
                    rotation += 6;
                    rotation %= 8;
                }
            } else {
                x = 2 * m_width - x - 1;
                y = 2 * m_height - y - 1;

                if (rotation) {
                    rotation += 4;
                    rotation %= 8;
                }
            }
        }
        return true;
    }
};

class ConeLeftTopTopology : public Topology {
public:
    ConeLeftTopTopology(int size) : Topology{size, size} {}

    Topology::Id getId() const override {
        return Topology::Id::CONE_LEFT_TOP;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        if (!sf::IntRect{-m_width, -m_height, 2 * m_width, 2 * m_height}.contains(x, y))
            return false;

        if (x < 0) {
            if (y < 0) {
                x = -x - 1, y = -y - 1;

                if (rotation) {
                    rotation += 4;
                    rotation %= 8;
                }
            } else {
                swap(x, y);
                y = -y - 1;

                if (rotation) {    
                    rotation += 2;
                    rotation %= 8;
                }
            }
        } else if (y < 0) {
            swap(x, y);
            x = -x - 1;

            if (rotation) {    
                rotation += 6;
                rotation %= 8;
            }
        }
        return true;
    }
};

class ConeRightTopTopology : public Topology {
public:
    ConeRightTopTopology(int size) : Topology{size, size} {}

    Topology::Id getId() const override {
        return Topology::Id::CONE_RIGHT_TOP;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        if (!sf::IntRect{0, -m_height, 2 * m_width, 2 * m_height}.contains(x, y))
            return false;

        if (x >= m_width) {
            if (y < 0) {
                x = 2 * m_width - x - 1, y = -y - 1;

                if (rotation) {
                    rotation += 4;
                    rotation %= 8;
                }
            } else {
                swap(x, y);
                x = m_width - x - 1, y = y - m_height;

                if (rotation) {    
                    rotation += 6;
                    rotation %= 8;
                }
            }
        } else if (y < 0) {
            swap(x, y);
            x = x + m_width, y = m_width - y - 1;

            if (rotation) {    
                rotation += 2;
                rotation %= 8;
            }
        }
        return true;
    }
};

class ConeLeftBottomTopology : public Topology {
public:
    ConeLeftBottomTopology(int size) : Topology{size, size} {}

    Topology::Id getId() const override {
        return Topology::Id::CONE_LEFT_BOTTOM;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        if (!sf::IntRect{-m_width, 0, 2 * m_width, 2 * m_height}.contains(x, y))
            return false;

        if (x < 0) {
            if (y >= m_height) {
                x = -x - 1, y = 2 * m_width - y - 1;

                if (rotation) {
                    rotation += 4;
                    rotation %= 8;
                }
            } else {
                swap(x, y);
                x = m_height - x - 1, y = y + m_height;

                if (rotation) {    
                    rotation += 6;
                    rotation %= 8;
                }
            }
        } else if (y >= m_height) {
            swap(x, y);
            x = x - m_width, y = m_height - y - 1;

            if (rotation) {    
                rotation += 2;
                rotation %= 8;
            }
        }
        return true;
    }
};

class ConeRightBottomTopology : public Topology {
public:
    ConeRightBottomTopology(int size) : Topology{size, size} {}

    Topology::Id getId() const override {
        return Topology::Id::CONE_RIGHT_BOTTOM;
    }
protected:
    bool do_makeIndicesSafe(int& x, int& y, int& rotation) const override {
        if (!sf::IntRect{0, 0, 2 * m_width, 2 * m_height}.contains(x, y))
            return false;

        if (x < m_width) {
            if (y >= m_height) {
                swap(x, y);
                x = 2 * m_width - x - 1;

                if (rotation) {
                    rotation += 6;
                    rotation %= 8;
                }
            }
        } else {
            if (y < m_height) {
                swap(x, y);
                y = 2 * m_height - y - 1;

                if (rotation) {    
                    rotation += 2;
                    rotation %= 8;
                }
            } else {
                x = 2 * m_width - x - 1;
                y = 2 * m_height - y - 1;

                if (rotation) {    
                    rotation += 4;
                    rotation %= 8;
                }
            }
        }
        return true;
    }
};

void Topology::showCombo(int fieldWidth, int fieldHeight, std::unique_ptr<Topology>& fieldTopology) {
    int id = static_cast<int>(Topology::Id::TORUS);
    if (fieldTopology)
        id = static_cast<int>(fieldTopology->getId());

    if (fieldWidth == fieldHeight) {
        ImGui::Combo("Topology", &id, 
            "Torus\0Cylinder X\0Cylinder Y\0Plane\0Sphere left\0Sphere right\0"
            "Cone left top\0Cone right top\0Cone left bottom\0Cone right bottom\0");
    } else {
        if (id > static_cast<int>(Topology::Id::PLANE))
            id = 0;
        ImGui::Combo("Topology", &id, "Torus\0Cylinder X\0Cylinder Y\0Plane\0");
    }

    fieldTopology = createTopology(static_cast<Id>(id), fieldWidth, fieldHeight);
}

std::unique_ptr<Topology> Topology::createTopology(Topology::Id id, int width, int height) {
    switch (id) {
        case Topology::Id::TORUS:             
            return std::make_unique<TorusTopology>(width, height);
        case Topology::Id::CYLINDER_X:        
            return std::make_unique<CylinderXTopology>(width, height);
        case Topology::Id::CYLINDER_Y:        
            return std::make_unique<CylinderYTopology>(width, height);
        case Topology::Id::PLANE:             
            return std::make_unique<PlaneTopology>(width, height);
        case Topology::Id::SPHERE_LEFT:       
            assert(width == height);
            return std::make_unique<SphereLeftTopology>(width);
        case Topology::Id::SPHERE_RIGHT:       
            assert(width == height);
            return std::make_unique<SphereRightTopology>(width);
        case Topology::Id::CONE_LEFT_TOP:       
            assert(width == height);
            return std::make_unique<ConeLeftTopTopology>(width);
        case Topology::Id::CONE_RIGHT_TOP:       
            assert(width == height);
            return std::make_unique<ConeRightTopTopology>(width);
        case Topology::Id::CONE_LEFT_BOTTOM:       
            assert(width == height);
            return std::make_unique<ConeLeftBottomTopology>(width);
        case Topology::Id::CONE_RIGHT_BOTTOM:       
            assert(width == height);
            return std::make_unique<ConeRightBottomTopology>(width);
        default: assert(false);
    }
}
