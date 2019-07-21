#ifndef EDIT_CONTROLLER_H
#define EDIT_CONTROLLER_H

#include <QWidget>

namespace Ui {
class ControllerWidget;
}

class ControllerWidgetImp : public QWidget
{
    Q_OBJECT
public:
    explicit ControllerWidgetImp(QWidget *parent = 0);
    ~ControllerWidgetImp();

signals:

public slots:
    void OnFrontBackValueChanged(int p_Value);
    void OnUpDownValueChanged(int p_Value);
    void OnLeftRightValueChanged(int p_Value);
    void OnRotationChanged(int p_Value);
    void OnAnchorXValueChanged(int p_Value);
    void OnAnchorYValueChanged(int p_Value);
    void OnZoomXChanged(int p_Value);
    void OnZoomYChanged(int p_Value);

    void OnResetClicked(bool p_Checked);

private:
    Ui::ControllerWidget* ui;
};

#endif // EDIT_CONTROLLER_H
