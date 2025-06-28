#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "exceptions.h"
#include <QMessageBox>
#include <QSplitter>
#include <QHeaderView>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sistema(new SistemaBovedas())
{
    ui->setupUi(this);
    
    // Configurar ventana principal
    setWindowTitle("Sistema de Gestión de Bóvedas Bancarias");
    setMinimumSize(1200, 800);
    
    // Configurar timer para actualizaciones automáticas
    timerActualizacion = new QTimer(this);
    connect(timerActualizacion, &QTimer::timeout, this, &MainWindow::actualizarDashboard);
    timerActualizacion->start(5000); // Actualizar cada 5 segundos
    
    setupUI();
    inicializarSistema();
    actualizarDashboard();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete sistema;
}

void MainWindow::setupUI() {
    // Widget central principal
    QWidget* centralWidget = new QWidget;
    setCentralWidget(centralWidget);
    
    // Layout principal horizontal (dashboard + panel de control)
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // Splitter para dividir el espacio
    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter);
    
    setupDashboard();
    setupControlPanel();
    
    // Agregar widgets al splitter
    splitter->addWidget(dashboardScroll);
    
    // Panel de control a la derecha
    QWidget* controlWidget = new QWidget;
    QVBoxLayout* controlMainLayout = new QVBoxLayout(controlWidget);
    
    // Panel de operaciones
    QGroupBox* operacionesGroup = new QGroupBox("Panel de Control - Operaciones");
    controlMainLayout->addWidget(operacionesGroup);
    
    QVBoxLayout* operacionesLayout = new QVBoxLayout(operacionesGroup);
    
    // Formulario de transferencia
    QGroupBox* transferenciaGroup = new QGroupBox("Nueva Transferencia");
    operacionesLayout->addWidget(transferenciaGroup);
    
    QGridLayout* formLayout = new QGridLayout(transferenciaGroup);
    
    // Banco y bóveda de origen
    formLayout->addWidget(new QLabel("Banco Origen:"), 0, 0);
    comboBancoOrigen = new QComboBox;
    formLayout->addWidget(comboBancoOrigen, 0, 1);
    
    formLayout->addWidget(new QLabel("Bóveda Origen:"), 1, 0);
    comboBovedaOrigen = new QComboBox;
    formLayout->addWidget(comboBovedaOrigen, 1, 1);
    
    // Banco y bóveda de destino
    formLayout->addWidget(new QLabel("Banco Destino:"), 2, 0);
    comboBancoDestino = new QComboBox;
    formLayout->addWidget(comboBancoDestino, 2, 1);
    
    formLayout->addWidget(new QLabel("Bóveda Destino:"), 3, 0);
    comboBovedaDestino = new QComboBox;
    formLayout->addWidget(comboBovedaDestino, 3, 1);
    
    // Tipo de activo y cantidad
    formLayout->addWidget(new QLabel("Tipo de Activo:"), 4, 0);
    comboTipoActivo = new QComboBox;
    comboTipoActivo->addItems({"Soles", "Dólares", "Joyas"});
    formLayout->addWidget(comboTipoActivo, 4, 1);
    
    formLayout->addWidget(new QLabel("Cantidad:"), 5, 0);
    lineCantidad = new QLineEdit;
    lineCantidad->setPlaceholderText("Ingrese la cantidad a transferir");
    formLayout->addWidget(lineCantidad, 5, 1);
    
    // Transportadora
    formLayout->addWidget(new QLabel("Transportadora:"), 6, 0);
    comboTransportadora = new QComboBox;
    comboTransportadora->addItem("Teletrans", "Teletrans");
    comboTransportadora->addItem("Prosegur", "Prosegur");
    comboTransportadora->addItem("Transportes Seguros SA", "Transportes Seguros SA");
    formLayout->addWidget(comboTransportadora, 6, 1);
    
    // Porcentaje de comisión
    formLayout->addWidget(new QLabel("Comisión (0.05-0.08):"), 7, 0);
    linePorcentajeComision = new QLineEdit;
    linePorcentajeComision->setPlaceholderText("0.05");
    linePorcentajeComision->setText("0.05");
    formLayout->addWidget(linePorcentajeComision, 7, 1);
    
    // Botón de transferencia
    btnIniciarTransferencia = new QPushButton("Iniciar Transferencia");
    formLayout->addWidget(btnIniciarTransferencia, 8, 0, 1, 2);
    
    // Panel de transacciones
    QGroupBox* transaccionesGroup = new QGroupBox("Gestión de Transacciones");
    operacionesLayout->addWidget(transaccionesGroup);
    
    QVBoxLayout* transaccionesLayout = new QVBoxLayout(transaccionesGroup);
    
    // Campo para ID de transacción
    QHBoxLayout* transaccionIdLayout = new QHBoxLayout;
    transaccionIdLayout->addWidget(new QLabel("ID Transacción:"));
    lineTransaccionId = new QLineEdit;
    lineTransaccionId->setPlaceholderText("TXN-XXXXXX");
    transaccionIdLayout->addWidget(lineTransaccionId);
    transaccionesLayout->addLayout(transaccionIdLayout);
    
    // Botón para procesar transacción
    btnProcesarTransaccion = new QPushButton("Procesar Transacción Completa");
    transaccionesLayout->addWidget(btnProcesarTransaccion);
    
    // Área de texto para mostrar transacciones
    textTransacciones = new QTextEdit;
    textTransacciones->setReadOnly(true);
    textTransacciones->setMaximumHeight(300);
    transaccionesLayout->addWidget(textTransacciones);
    
    splitter->addWidget(controlWidget);
    
    // Configurar proporciones del splitter
    splitter->setSizes({800, 400});
    
    // Conectar señales
    connect(btnIniciarTransferencia, &QPushButton::clicked, this, &MainWindow::iniciarTransferencia);
    connect(btnProcesarTransaccion, &QPushButton::clicked, this, &MainWindow::procesarTransaccion);
    connect(comboBancoOrigen, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onBancoOrigenChanged);
    connect(comboBancoDestino, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onBancoDestinoChanged);
    
    // Conectar para limpiar errores cuando el usuario interactúa
    connect(lineCantidad, &QLineEdit::textChanged, this, &MainWindow::limpiarError);
    connect(linePorcentajeComision, &QLineEdit::textChanged, this, &MainWindow::limpiarError);
    
    // Configurar barra de estado
    statusBar()->showMessage("Sistema inicializado correctamente", 3000);
}

void MainWindow::setupDashboard() {
    // Crear scroll area para el dashboard
    dashboardScroll = new QScrollArea;
    dashboardScroll->setWidgetResizable(true);
    dashboardScroll->setMinimumWidth(600);
    
    dashboardWidget = new QWidget;
    dashboardLayout = new QVBoxLayout(dashboardWidget);
    
    // Título del dashboard
    QLabel* titulo = new QLabel("Dashboard de Bancos Registrados");
    titulo->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    titulo->setAlignment(Qt::AlignCenter);
    dashboardLayout->addWidget(titulo);
    
    dashboardScroll->setWidget(dashboardWidget);
}

void MainWindow::setupControlPanel() {
    // Ya implementado en setupUI()
}

void MainWindow::inicializarSistema() {
    try {
        sistema->inicializarSistema();
        cargarDatosSistema();
        statusBar()->showMessage("Sistema inicializado con éxito", 3000);
    } catch (const BovedaException& e) {
        mostrarError(QString::fromStdString(e.what()));
    }
}

void MainWindow::cargarDatosSistema() {
    // Limpiar combos
    comboBancoOrigen->clear();
    comboBancoDestino->clear();
    
    // Cargar bancos
    const auto& bancos = sistema->getBancos();
    for (const auto& [codigo, banco] : bancos) {
        comboBancoOrigen->addItem(QString::fromStdString(banco->getNombre()), QString::fromStdString(codigo));
        comboBancoDestino->addItem(QString::fromStdString(banco->getNombre()), QString::fromStdString(codigo));
    }
    
    // Actualizar bóvedas
    onBancoOrigenChanged();
    onBancoDestinoChanged();
}

void MainWindow::actualizarDashboard() {
    // Limpiar layout anterior (excepto el título)
    while (dashboardLayout->count() > 1) {
        QLayoutItem* item = dashboardLayout->takeAt(1);
        if (item) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
    }
    
    try {
        const auto& bancos = sistema->getBancos();
        
        for (const auto& [codigo, banco] : bancos) {
            // Crear grupo para cada banco
            QGroupBox* bancoGroup = new QGroupBox(QString::fromStdString(banco->getNombre()));
            bancoGroup->setStyleSheet(
                "QGroupBox {"
                "    font-weight: bold;"
                "    font-size: 14px;"
                "    border: 2px solid #cccccc;"
                "    border-radius: 5px;"
                "    margin-top: 1ex;"
                "    padding: 10px;"
                "}"
                "QGroupBox::title {"
                "    subcontrol-origin: margin;"
                "    left: 10px;"
                "    padding: 0 5px 0 5px;"
                "}"
            );
            
            QVBoxLayout* bancoLayout = new QVBoxLayout(bancoGroup);
            
            // Información general del banco
            QLabel* infoGeneral = new QLabel(QString("Código: %1 | Activos Totales: $%2")
                                           .arg(QString::fromStdString(codigo))
                                           .arg(banco->getActivosTotales(), 0, 'f', 2));
            infoGeneral->setStyleSheet(
                "font-weight: normal;"
                "color: #2e7d32;"
                "font-size: 12px;"
                "margin-bottom: 5px;"
            );
            bancoLayout->addWidget(infoGeneral);
            
            // Información de bóvedas
            const auto& bovedas = banco->getBovedas();
            for (const auto& boveda : bovedas) {
                // Crear widget para cada bóveda
                QWidget* bovedaWidget = new QWidget;
                bovedaWidget->setStyleSheet(
                    "background-color: #f8f9fa;"
                    "border: 1px solid #dee2e6;"
                    "border-radius: 3px;"
                    "margin: 2px;"
                    "padding: 8px;"
                );
                
                QVBoxLayout* bovedaLayout = new QVBoxLayout(bovedaWidget);
                bovedaLayout->setContentsMargins(5, 5, 5, 5);
                
                // Título de la bóveda
                QLabel* tituloBovedea = new QLabel(QString("Bóveda: %1 (%2)")
                                                 .arg(QString::fromStdString(boveda->getId()))
                                                 .arg(QString::fromStdString(boveda->getUbicacion())));
                tituloBovedea->setStyleSheet("font-weight: bold; color: #495057;");
                bovedaLayout->addWidget(tituloBovedea);
                
                // Detalles de activos
                QLabel* soles = new QLabel(QString("  Soles: S/ %1")
                                         .arg(boveda->getSaldo(TipoActivo::SOLES), 0, 'f', 2));
                soles->setStyleSheet("font-family: monospace; font-size: 11px; color: #6c757d;");
                bovedaLayout->addWidget(soles);
                
                QLabel* dolares = new QLabel(QString("  Dólares: $ %1")
                                           .arg(boveda->getSaldo(TipoActivo::DOLARES), 0, 'f', 2));
                dolares->setStyleSheet("font-family: monospace; font-size: 11px; color: #6c757d;");
                bovedaLayout->addWidget(dolares);
                
                QLabel* joyas = new QLabel(QString("  Joyas: %1 unidades")
                                         .arg(boveda->getSaldo(TipoActivo::JOYAS), 0, 'f', 0));
                joyas->setStyleSheet("font-family: monospace; font-size: 11px; color: #6c757d;");
                bovedaLayout->addWidget(joyas);
                
                QLabel* valorTotal = new QLabel(QString("  Valor total: $ %1")
                                              .arg(boveda->getValorTotalEnDolares(), 0, 'f', 2));
                valorTotal->setStyleSheet("font-family: monospace; font-size: 11px; font-weight: bold; color: #28a745;");
                bovedaLayout->addWidget(valorTotal);
                
                bancoLayout->addWidget(bovedaWidget);
            }
            
            dashboardLayout->addWidget(bancoGroup);
        }
        
        // Agregar información de transacciones
        QGroupBox* transaccionesGroup = new QGroupBox("Estado de Transacciones");
        transaccionesGroup->setStyleSheet(
            "QGroupBox {"
            "    font-weight: bold;"
            "    font-size: 14px;"
            "    border: 2px solid #17a2b8;"
            "    border-radius: 5px;"
            "    margin-top: 1ex;"
            "    padding: 10px;"
            "}"
            "QGroupBox::title {"
            "    subcontrol-origin: margin;"
            "    left: 10px;"
            "    padding: 0 5px 0 5px;"
            "    color: #17a2b8;"
            "}"
        );
        
        QVBoxLayout* transaccionesLayout = new QVBoxLayout(transaccionesGroup);
        
        auto transacciones = sistema->getTodasLasTransacciones();
        QLabel* estadoTransacciones = new QLabel(QString("Transacciones Totales: %1").arg(transacciones.size()));
        estadoTransacciones->setStyleSheet("font-weight: normal; color: #495057;");
        transaccionesLayout->addWidget(estadoTransacciones);
        
        // Contar transacciones por estado
        int activas = 0, completadas = 0, canceladas = 0;
        for (const auto& t : transacciones) {
            if (t->estaCompletada()) completadas++;
            else if (t->getEstado() == EstadoTransaccion::CANCELADA) canceladas++;
            else activas++;
        }
        
        QLabel* detalleTransacciones = new QLabel(QString("Activas: %1 | Completadas: %2 | Canceladas: %3")
                                                .arg(activas).arg(completadas).arg(canceladas));
        detalleTransacciones->setStyleSheet("font-weight: normal; color: #6c757d; font-size: 11px;");
        transaccionesLayout->addWidget(detalleTransacciones);
        
        // Mostrar últimas transacciones
        QString textoTransacciones;
        for (size_t i = 0; i < std::min(size_t(3), transacciones.size()); ++i) {
            auto& transaccion = transacciones[transacciones.size() - 1 - i]; // Últimas primero
            textoTransacciones += QString("ID: %1 | Estado: %2 | %3 %4\n")
                                .arg(QString::fromStdString(transaccion->getId()))
                                .arg(QString::fromStdString(transaccion->getEstadoString()))
                                .arg(transaccion->getActivo().getCantidad(), 0, 'f', 2)
                                .arg(QString::fromStdString(transaccion->getActivo().getTipoString()));
        }
        
        textTransacciones->setPlainText(textoTransacciones);
        
        dashboardLayout->addWidget(transaccionesGroup);
        dashboardLayout->addStretch();
        
        // Actualizar el widget
        dashboardWidget->update();
        
    } catch (const BovedaException& e) {
        mostrarError(QString::fromStdString(e.what()));
    }
}

void MainWindow::onBancoOrigenChanged() {
    actualizarComboBovedas(comboBancoOrigen, comboBovedaOrigen);
}

void MainWindow::onBancoDestinoChanged() {
    actualizarComboBovedas(comboBancoDestino, comboBovedaDestino);
}

void MainWindow::actualizarComboBovedas(QComboBox* comboBanco, QComboBox* comboBoveda) {
    comboBoveda->clear();
    
    if (comboBanco->currentIndex() < 0) return;
    
    QString codigoBanco = comboBanco->currentData().toString();
    
    try {
        Banco* banco = sistema->buscarBanco(codigoBanco.toStdString());
        const auto& bovedas = banco->getBovedas();
        
        for (const auto& boveda : bovedas) {
            QString texto = QString("%1 (%2)").arg(QString::fromStdString(boveda->getId()))
                                             .arg(QString::fromStdString(boveda->getUbicacion()));
            comboBoveda->addItem(texto, QString::fromStdString(boveda->getId()));
        }
    } catch (const BovedaException& e) {
        mostrarError(QString::fromStdString(e.what()));
    }
}

void MainWindow::iniciarTransferencia() {
    try {
        limpiarError();
        
        // Validar que todos los campos estén llenos
        if (comboBancoOrigen->currentIndex() < 0 || comboBovedaOrigen->currentIndex() < 0 ||
            comboBancoDestino->currentIndex() < 0 || comboBovedaDestino->currentIndex() < 0) {
            throw DatosInvalidosException("Debe seleccionar banco y bóveda de origen y destino");
        }
        
        if (lineCantidad->text().isEmpty()) {
            throw DatosInvalidosException("Debe ingresar una cantidad a transferir");
        }
        
        if (linePorcentajeComision->text().isEmpty()) {
            throw DatosInvalidosException("Debe ingresar un porcentaje de comisión");
        }
        
        // Obtener datos del formulario
        QString bancoOrigen = comboBancoOrigen->currentData().toString();
        QString bovedaOrigen = comboBovedaOrigen->currentData().toString();
        QString bancoDestino = comboBancoDestino->currentData().toString();
        QString bovedaDestino = comboBovedaDestino->currentData().toString();
        
        QString tipoActivoStr = comboTipoActivo->currentText();
        TipoActivo tipoActivo = Activo::stringToTipoActivo(tipoActivoStr.toStdString());
        
        QString transportadora = comboTransportadora->currentData().toString();
        
        bool ok;
        double cantidad = lineCantidad->text().toDouble(&ok);
        if (!ok || cantidad <= 0) {
            throw DatosInvalidosException("La cantidad debe ser un número positivo");
        }
        
        double porcentajeComision = linePorcentajeComision->text().toDouble(&ok);
        if (!ok || porcentajeComision < 0.05 || porcentajeComision > 0.08) {
            throw DatosInvalidosException("El porcentaje de comisión debe estar entre 0.05 (5%) y 0.08 (8%)");
        }
        
        // Iniciar transferencia
        std::string transaccionId = sistema->iniciarTransferencia(
            bancoOrigen.toStdString(),
            bovedaOrigen.toStdString(),
            bancoDestino.toStdString(),
            bovedaDestino.toStdString(),
            tipoActivo,
            cantidad,
            transportadora.toStdString(),
            porcentajeComision
        );
        
        // Limpiar formulario
        lineCantidad->clear();
        
        // Mostrar mensaje de éxito
        statusBar()->showMessage(QString("Transferencia iniciada con ID: %1").arg(QString::fromStdString(transaccionId)), 5000);
        lineTransaccionId->setText(QString::fromStdString(transaccionId));
        
        // Actualizar dashboard
        actualizarDashboard();
        
    } catch (const BovedaException& e) {
        mostrarError(QString::fromStdString(e.what()));
    } catch (const std::exception& e) {
        mostrarError(QString("Error inesperado: %1").arg(e.what()));
    }
}

void MainWindow::procesarTransaccion() {
    try {
        limpiarError();
        
        QString transaccionId = lineTransaccionId->text().trimmed();
        if (transaccionId.isEmpty()) {
            throw DatosInvalidosException("Debe ingresar un ID de transacción");
        }
        
        sistema->procesarTransaccion(transaccionId.toStdString());
        
        statusBar()->showMessage(QString("Transacción %1 procesada completamente").arg(transaccionId), 5000);
        lineTransaccionId->clear();
        
        // Actualizar dashboard
        actualizarDashboard();
        
    } catch (const BovedaException& e) {
        mostrarError(QString::fromStdString(e.what()));
    } catch (const std::exception& e) {
        mostrarError(QString("Error inesperado: %1").arg(e.what()));
    }
}

void MainWindow::mostrarError(const QString& mensaje) {
    statusBar()->setStyleSheet("QStatusBar { color: red; font-weight: bold; }");
    statusBar()->showMessage("ERROR: " + mensaje, 10000);
}

void MainWindow::limpiarError() {
    statusBar()->setStyleSheet("");
}
