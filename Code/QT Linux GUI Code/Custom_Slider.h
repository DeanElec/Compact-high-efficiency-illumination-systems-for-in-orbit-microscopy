#ifndef CUSTOM_SLIDER_H
#define CUSTOM_SLIDER_H



#include <QSlider>
#include <QStyleOptionSlider>
#include <QMouseEvent>

class Custom_Slider : public QSlider {
    Q_OBJECT

public:
    using QSlider::QSlider; // Inherit constructors

protected:
    void mousePressEvent(QMouseEvent *event) override {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect handleRect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        if (event->button() == Qt::LeftButton && handleRect.contains(event->pos())) {
            //if the click is on the handle, let the base class implementation process it
            QSlider::mousePressEvent(event);
        } else {
            //ignore clicks on the slider track that are not on the handle
            event->ignore();
        }
    }
};

#endif // CUSTOM_SLIDER_H
