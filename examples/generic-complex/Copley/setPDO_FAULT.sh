#!/bin/bash

POS=-p0
echo 'clear and set RxPdo'
ethercat $POS --type uint8 download 0x1C12 0 0
ethercat $POS --type uint8 download 0x1600 0 0
ethercat $POS --type uint8 download 0x1601 0 0
ethercat $POS --type uint8 download 0x1602 0 0
ethercat $POS --type uint8 download 0x1603 0 0
ethercat $POS --type uint32 download 0x1600 1 0x60400010
ethercat $POS --type uint32 download 0x1600 2 0x60ff0020
ethercat $POS --type uint32 download 0x1600 3 0x21830020
ethercat $POS --type uint8 download 0x1600 0 3
ethercat $POS --type uint16 download 0x1C12 1 0x1600
ethercat $POS --type uint8  download 0x1C12 0 1

echo 'clear and set TxPdo'
ethercat $POS --type uint8  download 0x1C13 0 0
ethercat $POS --type uint8 download 0x1A00 0 0
ethercat $POS --type uint8 download 0x1A01 0 0
ethercat $POS --type uint8 download 0x1A02 0 0
ethercat $POS --type uint8 download 0x1A03 0 0
ethercat $POS --type uint32 download 0x1A00 1 0x60410010
ethercat $POS --type uint32 download 0x1A00 2 0x606c0020 # 
ethercat $POS --type uint32 download 0x1A00 3 0x21830020 # latch status
ethercat $POS --type uint32 download 0x1A00 4 0x60ff0020
ethercat $POS --type uint32 download 0x1A00 5 0x21820020 # latch mask 

ethercat $POS --type uint8 download 0x1A00 0 5 # number of var in this PDO
ethercat $POS --type uint16  download 0x1C13 1 0x1A00 # list TxPdo
ethercat $POS --type uint8  download 0x1C13 0 1 # number of TxPdo
ethercat rescan
sleep 3
ethercat pdos
#dmesg | tail -50


echo "Set DESIRED STATE 0X2300 to 30 In servo mode, the position loop is driven by the CANopen interface."
ethercat -p0 --type uint16 download  0x2300 0 30

echo "Set MODE OF OPERATION 0X6060 3 Profile Velocity mode on"
ethercat -p0 --type int8 download 0x6060 0 3

echo "Get Status word STATUS WORD 0X6041"
ethercat upload -p0 -tuint16 0x6041 0

echo "Reset  Fault: Bit 7 on CONTROL WORD 0X6040"
ethercat -p0 --type uint16 download 0X6040 0 0x0080

echo "Get Status word STATUS WORD 0X6041"
ethercat upload -p0 -tuint16 0x6041 0

echo "Set Target Velocity 0X60FF to 3000"
ethercat -p0 --type uint32 download 0x60FF 0 3000

echo "Get Status word STATUS WORD 0X6041"
ethercat upload -p0 -tuint16 0x6041 0

echo "Reset CONTROL WORD 0X6040 to Switch On, Enable Voltage, Quick Stop Not Clear, Enable Operation"
ethercat -p0 --type uint16 download 0X6040 0 0x000f

echo "Get actual velocity 0X6069"
ethercat upload -p0 -tint32 0X6069 0

