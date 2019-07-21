#include "TransformEditView.h"
#include "ui_MyControllerWidget.h"

ControllerWidgetImp::ControllerWidgetImp(QWidget *parent) : QWidget(parent),
        ui(new Ui::ControllerWidget)
{
    ui->setupUi(this);
    connect(ui->m_FrontBackSlider, SIGNAL(valueChanged(int)), this, SLOT(OnFrontBackValueChanged(int)));
    connect(ui->m_UpDownSlider, SIGNAL(valueChanged(int)), this, SLOT(OnUpDownValueChanged(int)));
    connect(ui->m_LeftRightSlider, SIGNAL(valueChanged(int)), this, SLOT(OnLeftRightValueChanged(int)));
    connect(ui->m_RotationSlider, SIGNAL(valueChanged(int)), this, SLOT(OnRotationChanged(int)));
    connect(ui->m_AnchorXSlider, SIGNAL(valueChanged(int)), this, SLOT(OnAnchorXValueChanged(int)));
    connect(ui->m_AnchorYSlider, SIGNAL(valueChanged(int)), this, SLOT(OnAnchorYValueChanged(int)));
    connect(ui->m_ZoomXSlider, SIGNAL(valueChanged(int)), this, SLOT(OnZoomXChanged(int)));
    connect(ui->m_ZoomYSlider, SIGNAL(valueChanged(int)), this, SLOT(OnZoomYChanged(int)));
    connect(ui->m_ResetButton, SIGNAL(clicked(bool)), this, SLOT(OnResetClicked(bool)));
}

ControllerWidgetImp::~ControllerWidgetImp()
{
    delete ui;
}

void ControllerWidgetImp::OnFrontBackValueChanged(int p_Value)
{
    ui->m_OpenGLWidget->GetControlParams()->m_FrontBack = p_Value;
}

void ControllerWidgetImp::OnUpDownValueChanged(int p_Value)
{
    ui->m_OpenGLWidget->GetControlParams()->m_PanY = p_Value;
}

void ControllerWidgetImp::OnLeftRightValueChanged(int p_Value)
{
    ui->m_OpenGLWidget->GetControlParams()->m_PanX = p_Value;
}

void ControllerWidgetImp::OnRotationChanged(int p_Value)
{
    ui->m_OpenGLWidget->GetControlParams()->m_Rotate = p_Value;
}

void ControllerWidgetImp::OnAnchorXValueChanged(int p_Value)
{
    ui->m_OpenGLWidget->m_TransformWindow->m_AnchorPoint->m_PosX = p_Value;
}

void ControllerWidgetImp::OnAnchorYValueChanged(int p_Value)
{
    ui->m_OpenGLWidget->m_TransformWindow->m_AnchorPoint->m_PosY = p_Value;
}

void ControllerWidgetImp::OnZoomXChanged(int p_Value)
{
    ui->m_OpenGLWidget->GetControlParams()->m_ZoomX = p_Value / 100.f;
}

void ControllerWidgetImp::OnZoomYChanged(int p_Value)
{
    ui->m_OpenGLWidget->GetControlParams()->m_ZoomY = p_Value / 100.f;
}

void ControllerWidgetImp::OnResetClicked(bool p_Checked)
{
    ui->m_FrontBackSlider->setValue(0);
    ui->m_UpDownSlider->setValue(0);
    ui->m_LeftRightSlider->setValue(0);

    ui->m_RotationSlider->setValue(0);
    ui->m_ZoomXSlider->setValue(100);
    ui->m_ZoomYSlider->setValue(100);
    ui->m_AnchorXSlider->setValue(0);
    ui->m_AnchorYSlider->setValue(0);
}
