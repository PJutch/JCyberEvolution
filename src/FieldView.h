#ifndef FIELD_VIEW_H_
#define FIELD_VIEW_H_

#include "Field.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class FieldView : public sf::Drawable {
public:
    FieldView(sf::FloatRect rect, sf::FloatRect viewport, Field& field);

    bool handleEvent(const sf::Event& event) noexcept;

    void update(bool keyboardAvailable, sf::Time elapsedTime) noexcept;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override {
        sf::View prevView = target.getView();
        target.setView(m_view);
        target.draw(m_field, states);
        target.setView(prevView);
    }
private:
    Field& m_field;
    sf::View m_view;
    float m_zoom;

    float m_baseZoomingChange;
    float m_baseMovingSpeed;
    float m_speedModificator;
};

#endif