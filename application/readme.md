# SPIRAW Software Architecture

## Core Architecture Pattern
**Hierarchical State Machine (HSM) with FreeRTOS** - Event-driven architecture with clear state separation and real-time task management.

## High-Level Structure
```
Application Layer
├── State Machine Engine (Main Controller)
│   ├── System States (Setup, Culture, Harvest, Clean, Maintenance)
│   ├── State Transitions & Guards
│   └── Event Dispatcher
├── GUI Controller (TouchGFX Interface)
├── Sensor Manager
├── Actuator Controller
└── Data Logger & Analytics
```

## Communication Architecture
- **Inter-task Communication**: FreeRTOS Queues, Semaphores, Event Groups
- **Hardware Abstraction**: STM32 HAL drivers
- **WiFi Communication**: UART to ESP32 module
- **Data Format**: JSON/Binary for network transmission

## State Machine Design

### Main State Structure
```
SPIRAW_SYSTEM
├── IDLE
│   └── Substates: POWER_ON, READY
├── SETUP
│   ├── WIFI_CONFIGURATION
│   ├── WATER_FILLING
│   ├── ADD_NUTRIENTS
│   ├── MICROPUMP_MIXING
│   ├── UVC_STERILIZATION
│   └── ADD_STARTER
├── CULTURE
│   ├── MONITORING
│   ├── ENVIRONMENTAL_CONTROL
│   └── GROWTH_TRACKING
├── HARVEST_READY
├── HARVESTING
│   ├── FILTRATION_PROCESS
│   ├── COLLECTION_PROCESS
│   └── VOLUME_VERIFICATION
├── CLEANING
│   ├── SYSTEM_FLUSH
│   ├── STERILIZATION
│   └── DRYING
└── MAINTENANCE
    ├── MANUAL_CONTROL
    ├── DIAGNOSTICS
    └── CALIBRATION
```

### State Transitions
```
IDLE → SETUP (User initiates setup)
SETUP → CULTURE (Setup complete)
CULTURE → HARVEST_READY (Turbidity = 100% OR 3 months passed)
HARVEST_READY → HARVESTING (User confirms OR Auto-harvest enabled)
HARVESTING → CLEANING (Harvest complete)
CLEANING → CULTURE (Cleaning complete)
ANY_STATE → MAINTENANCE (User request OR System alert)
```

### Event Types
```cpp
typedef enum {
    // User Events
    USER_SETUP_START,
    USER_HARVEST_CONFIRM,
    USER_MAINTENANCE_REQUEST,
    
    // System Events
    SETUP_COMPLETE,
    TURBIDITY_THRESHOLD_REACHED,
    HARVEST_VOLUME_ACHIEVED,
    CLEANING_COMPLETE,
    
    // Error Events
    SENSOR_FAILURE,
    ACTUATOR_FAILURE,
    SYSTEM_ERROR
} SystemEvent_t;
```

## FreeRTOS Task Architecture

### Task Hierarchy & Priorities
```
State Machine Task (Priority: 5 - Highest)
├── Controls → WiFi Task (Priority: 3)
├── Controls → Sensor Task (Priority: 4)
├── Controls → Actuator Task (Priority: 4)
├── Controls → GUI Task (Priority: 3)
├── Coordinates → Data Logger Task (Priority: 2)
└── Monitors → Watchdog Task (Priority: 6 - Critical)
```

### Task Responsibilities

#### 1. State Machine Task (Main Orchestrator)
```cpp
void StateMachineTask(void *pvParameters)
{
    SystemEvent_t event;
    while(1) {
        // Wait for events from other tasks or GUI
        if(xQueueReceive(stateMachineQueue, &event, portMAX_DELAY)) {
            processSystemEvent(&event);
            updateSystemState();
            notifyRelevantTasks();
        }
    }
}
```

#### 2. Sensor Task
```cpp
void SensorTask(void *pvParameters)
{
    SensorData_t sensorData;
    while(1) {
        switch(currentSystemState) {
            case STATE_CULTURE:
                readAllSensors(&sensorData);
                validateSensorData(&sensorData);
                sendToDataLogger(&sensorData);
                checkAlarmConditions(&sensorData);
                vTaskDelay(pdMS_TO_TICKS(300000)); // 5 minutes
                break;
            case STATE_HARVEST:
                readTurbiditySensor();
                readFlowMeters();
                vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second
                break;
            default:
                vTaskDelay(pdMS_TO_TICKS(10000)); // 10 seconds
        }
    }
}
```

#### 3. Actuator Task
```cpp
void ActuatorTask(void *pvParameters)
{
    ActuatorCommand_t command;
    while(1) {
        if(xQueueReceive(actuatorQueue, &command, portMAX_DELAY)) {
            executeActuatorCommand(&command);
            reportCommandStatus();
        }
    }
}
```

#### 4. GUI Task (TouchGFX)
```cpp
void GuiTask(void *pvParameters)
{
    touchgfx_init();
    while(1) {
        touchgfx_taskEntry(); // TouchGFX main loop
        vTaskDelay(pdMS_TO_TICKS(16)); // ~60 FPS
    }
}
```

#### 5. WiFi Task
```cpp
void WiFiTask(void *pvParameters)
{
    WiFiCommand_t wifiCmd;
    while(1) {
        if(xQueueReceive(wifiQueue, &wifiCmd, portMAX_DELAY)) {
            switch(wifiCmd.type) {
                case WIFI_SCAN:
                    performWiFiScan();
                    break;
                case WIFI_CONNECT:
                    connectToNetwork(wifiCmd.ssid, wifiCmd.password);
                    break;
                case WIFI_SEND_DATA:
                    transmitSensorData(wifiCmd.data);
                    break;
            }
        }
    }
}
```

### Inter-Task Communication
```cpp
// Queues for task communication
QueueHandle_t stateMachineQueue;     // Events to state machine
QueueHandle_t actuatorQueue;         // Commands to actuators
QueueHandle_t sensorDataQueue;       // Sensor data distribution
QueueHandle_t wifiQueue;             // WiFi operations
QueueHandle_t guiEventQueue;         // GUI events

// Shared data structures
typedef struct {
    SystemState_t currentState;
    SystemState_t previousState;
    uint32_t stateEntryTime;
    bool errorCondition;
} SystemStatus_t;

extern volatile SystemStatus_t systemStatus;
```

## Implementation Steps

### Phase 1: TouchGFX Foundation
1. **Create Welcome Screen**
   ```
   - Background gradient design
   - SPIRAW logo and branding
   - Setup button with animation
   - Navigation to WiFi screens
   ```

2. **Complete Setup Screens**
   ```
   - Water filling progress screen
   - Nutrient addition guide
   - Mixing process display
   - UVC sterilization timer
   - Starter culture addition
   ```

3. **Generate Code Framework**
   ```
   - TouchGFX code generation
   - View/Presenter pattern setup
   - Screen transition logic
   ```

### Phase 2: State Machine Core
1. **Basic State Structure**
   ```cpp
   typedef enum {
       STATE_IDLE,
       STATE_SETUP,
       STATE_CULTURE,
       STATE_HARVEST_READY,
       STATE_HARVESTING,
       STATE_CLEANING,
       STATE_MAINTENANCE
   } SystemState_t;
   ```

2. **Event Processing**
   ```cpp
   void processSystemEvent(SystemEvent_t* event) {
       switch(currentState) {
           case STATE_IDLE:
               handleIdleState(event);
               break;
           case STATE_SETUP:
               handleSetupState(event);
               break;
           // ... other states
       }
   }
   ```

3. **Task Creation**
   ```cpp
   // Create FreeRTOS tasks
   xTaskCreate(StateMachineTask, "StateMachine", 1024, NULL, 5, &stateMachineHandle);
   xTaskCreate(SensorTask, "Sensors", 1024, NULL, 4, &sensorTaskHandle);
   xTaskCreate(ActuatorTask, "Actuators", 1024, NULL, 4, &actuatorTaskHandle);
   xTaskCreate(GuiTask, "GUI", 2048, NULL, 3, &guiTaskHandle);
   xTaskCreate(WiFiTask, "WiFi", 1024, NULL, 3, &wifiTaskHandle);
   ```

### Phase 3: Hardware Integration
1. **Sensor Implementation**
   - ADC configuration and reading
   - Data validation and filtering
   - Alarm threshold monitoring

2. **Actuator Control**
   - Relay channel management
   - Stepper motor control
   - PWM for variable devices

3. **Safety Systems**
   - Emergency stop functionality
   - Sensor fault detection
   - Actuator feedback verification

### Phase 4: Process Automation
1. **Culture Process**
   - 12h/12h lighting cycle
   - Continuous ventilation
   - Temperature regulation
   - Growth monitoring

2. **Harvest Process**
   - Turbidity-based triggering
   - Automated filtration
   - Volume measurement
   - Collection sequence

3. **Cleaning Cycle**
   - Multi-step cleaning process
   - Sterilization procedures
   - System flush operations

## Key Design Principles

### 1. Safety First
- All critical operations have manual overrides
- Multiple sensor validation
- Fail-safe defaults
- Emergency stop capability

### 2. User Experience
- Intuitive TouchGFX interface
- Clear process visualization
- Helpful guidance messages
- Error explanation and recovery

### 3. Maintainability
- Modular architecture
- Clear code organization
- Comprehensive logging
- Diagnostic capabilities

### 4. Scalability
- Easy to add new sensors
- Expandable state machine
- Configurable parameters
- Future feature integration