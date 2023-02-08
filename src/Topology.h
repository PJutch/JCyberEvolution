/* This file is part of JCyberEvolution.

JCyberEvolution is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the Free Software Foundation, 
either version 3 of the License, or (at your option) any later version.

JCyberEvolution is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with JCyberEvolution. 
If not, see <https://www.gnu.org/licenses/>. */

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include <memory>

class Field;

class Topology {
public:
    Topology(int width, int height) noexcept : m_width{width}, m_height{height} {}

    bool makeIndicesSafe(int& x, int& y, int& rotation) const {
        rotation %= 8;
        return do_makeIndicesSafe(x, y, rotation);
    }

    bool makeIndicesSafe(int& x, int& y) const {
        int rotation = 0;
        return do_makeIndicesSafe(x, y, rotation);
    }

    enum class Id {
        TORUS = 0,
        CYLINDER_X,
        CYLINDER_Y,
        PLANE,
        SPHERE_LEFT,
        SPHERE_RIGHT,
        CONE_LEFT_TOP,
        CONE_RIGHT_TOP,
        CONE_LEFT_BOTTOM,
        CONE_RIGHT_BOTTOM
    };
    virtual Id getId() const = 0;

    static void showCombo(int fieldWidth, int fieldHeight, std::unique_ptr<Topology>& fieldTopology);
protected:
    int m_width;
    int m_height;

    virtual bool do_makeIndicesSafe(int& x, int& y, int& rotation) const = 0;
private:
    static std::unique_ptr<Topology> createTopology(Id id, int width, int height);
};

#endif
