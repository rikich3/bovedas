#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QLabel>
#include <QTextEdit>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QStatusBar>
#include <QScrollArea>
#include <QTimer>
#include "sistema_bovedas.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void iniciarTransferencia();
    void procesarTransaccion();
    void actualizarDashboard();
    void onBancoOrigenChanged();
    void onBancoDestinoChanged();
    void mostrarError(const QString& mensaje);
    void limpiarError();

private:
    Ui::MainWindow *ui;
    SistemaBovedas* sistema;
    QTimer* timerActualizacion;
    
    // Widgets del dashboard
    QScrollArea* dashboardScroll;
    QWidget* dashboardWidget;
    QVBoxLayout* dashboardLayout;
    
    // Widgets del formulario de control
    QComboBox* comboBancoOrigen;
    QComboBox* comboBovedaOrigen;
    QComboBox* comboBancoDestino;
    QComboBox* comboBovedaDestino;
    QComboBox* comboTipoActivo;
    QComboBox* comboTransportadora;
    QLineEdit* lineCantidad;
    QLineEdit* linePorcentajeComision;
    QPushButton* btnIniciarTransferencia;
    
    // Widgets de transacciones
    QTextEdit* textTransacciones;
    QPushButton* btnProcesarTransaccion;
    QLineEdit* lineTransaccionId;
    
    void setupUI();
    void setupDashboard();
    void setupControlPanel();
    void inicializarSistema();
    void actualizarComboBovedas(QComboBox* comboBanco, QComboBox* comboBoveda);
    void cargarDatosSistema();
};
#endif // MAINWINDOW_H
