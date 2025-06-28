``` c
/*
    Copyright (C) 2025  Giang Trinh

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
```

# Project Layered Architecture
## Layer 1: **System architecture infrastructure**
- [Reference libraries](./components/_00_reference_libs/README.md)
    - List of reference libraries used in this project
- [Container structures]
    - Initialize the container structures needed to communicate between tasks
- [Task sync tools]
    - Initialize tasks synchronization tools
## Layer 2: **Peripheral connections**
- Layer 2_1:
    - [Internal peripherals]
        - Initialize this microcontroller available peripherals and wifi driver
        - Using `Task sync tools` and is called by upper layer devices `Layer 3` when communication with internal peripherals is required.
    - `List designed`: i2c, spi, ledc, mcpwm, gpio, wifi 
    - `List Coming soon`: adc, i2s, uart, rmt
- Layer 2_2:
    - [External peripherals]
        - External peripherals extend the capabilities of `internal peripherals`
        - Using `Task sync tools` and is called by upper layer devices `Layer 3` when communication with external peripherals is required.
    - `List designed`: 74HC165
    - `Coming soon`: 74HC565, 74HC4067, rs485, i2s_max98537
- Layer 2_3:
    - [Internal devices]
        - These are add-on devices that extend the capabilities of the microcontroller
        - Communicate with microcontroller via peripheral (internal or external) `Layer 2_1`
        - Using `Task sync tools` and these libraries API will be called by driver in `Layer 4`
        - *Warn:*
            - Internal devices can register some special things of `peripherals`, avoid register the same things have been used.
            - These device design for general purpose of User define driver `Layer 4_1_2` and should manage by `Widget driver` of layer `4_1_1`
            - Although these devices can be access by any driver but only one `Widget driver` manage interact with these device reduce conflict risk when multiple drivers are involved in controlling the device
    - `List designed`: ssd1306, spi_sd_card
    - `Coming soon`: :v nothing now, missing ds3231
- Layer 2_4:
    - [Network application protocol]
        - *Note:* initialize wifi before launch application protocols
        - Initialize drivers that launch network protocols
        - Using `Task sync tools` and is called by special drivers for each protocol at `Layer 4_1_1` when communication with network is required.
    - `List designed`: HTTP, MQTT
    - `Coming soon`: ModbusRTU
## Layer 3: **User define IO devices**
- [External devices]
    - These are `user defined` devices.
    - Communicate with microcontroller via peripheral (internal or external) `Layer 2_1`
    - *Warn:* 
        - Avoid using conflicts `peripherals` with `Internal devices`
    - These libraries API will be called by `External drivers` in `Layer 4_1_2`
- `List designed`: Coming soon
- `Coming soon`: :v nothing now
## Layer 4: **Initiate Controller drivers and User define drivers**
- Layer 4_1_1:
    - [Internal drivers]
        - These drivers manage network and container, timer, maintain and reset, clean & reset factory MCU
        - Communicate with `External driver` asynchronous or synchronous through container structures in `Layer 1`
        - *Warn:*
            - These driver not allowed to control `External devices` at `Layer 3_1_2`
    - `List designed`: Coming soon
    - `Coming soon`:
        - `System driver`: update, sleep, reset factory
        - `Widget driver`: Network, Timer, Read/Write file, Display, Speaker player, Email, Real time update
- Layer 4_1_2:
    - [External drivers]
        - *Compulsory:*
            - Handle communicate with `Internal drivers` at `Layer 4_1_1`
            - And define logic control for available `IO devices` and `peripheral`
        - *Warn:*
            - These driver should initial after constructed `Internal driver`, because they manager modify container comunicate data
    - `List designed`: Coming soon
    - `Coming soon`: :v nothing now

## Layer top: **Deploy initialization**
- **Sequential initialization**
    - Layer 1: 
        - Initialize container structure
        - Initialize synchorous tools
    - Layer 2: System resource
        - Initialize internal peripherals
        - Initialize external peripherals
        - Initialize internal IO device
        - Initialize network (OSI model: Physic (wifi), Datalink(Mac), Network(IP), Transport(TCP))
    - Layer 3: User define devices
        - Initialize external IO device
    - Layer 4: System drivers and user drivers
        - Initialize internal control driver
            - System driver:
                - Maintain system driver ( update, sleep, reset factory )
            - Widget driver:
                - Timer driver ( scheduling with clock device )
                - Network driver (implement protocol - Application layer in TCP/IP model)
                - Speaker player driver
                    - Check `Read/Write file driver` before run file
                    - Ensures synchronized audio `file` playback through a single device
                    - Limit only 1 audio `file` playback ( reduce file must open for File system )
                    - *Note:*
                        - File play back must be existed
                        - SPI_SD_CARD_MAX_FILE_OPEN limit number file can open at the same time
                - Read/Write file driver
                    - Check `speaker driver` before write to file (fail if file be using)
                    - Only read/write at the same time because SPI_SD_CARD_MAX_FILE_OPEN limit number file can open at the same time
                - Display driver
                    - Display performance ( RAM / log %, bar )
                    - Print new debug (time / error log)
        - Initialize external control driver
            - User define
    - Loop waiting / blocked until end signal from special internal system driver