/*
 * Created & Collected by Tamal Patra <patra.tamal@gmail.com>. Copyright 2016
 * Released for the sake of knowledge for mankind under The MIT License.
 */

/* 
 * File:   MatrixCharlieplex.cpp
 * Description: Drive (N * (N-1)) number of "Charlieplxed" diode matrices
 *              (e.g. LED Matrix) with N number of outputs capable of tri-state.
 *              It
 *
 *                   C A T H O D E
 *                   1 2 3 4 5 . N
 *              A  1 X 0 0 0 0
 *              N  2 0 X 0 0 0
 *              O  3 0 0 X 0 0
 *              D  4 0 0 0 X 0
 *              E  5 0 0 0 0 X
 *                 .           X
 *                 N             X
 * 
 * Author: Tamal Patra <patra.tamal@gmail.com>
 * 
 * Created on April 1, 2016, 12:00 PM
 */

#include "Arduino.h"
#include "MatrixCharlieplex.h"

namespace MatrixCharlieplex {
#pragma region Constructor

    MatrixCharlieplex::MatrixCharlieplex(uint8_t pins[]) {
        this->_pins = pins;
        this->_noOfPins = sizeof (pins) / sizeof (uint8_t);
        this->_maxNode = this->_noOfPins * (this->_noOfPins - 1);
        this->_activeNode = new DiodeNode();
        this->_init(false);
    }
#pragma endregion Constructor

#pragma region Private Functions

    MatrixCharlieplex::_init(boolean isReset) {
        // Sink all output pins
        for (uint8_t i = 0; i < this->_noOfPins; i++) {
            _sinkPin(*(this->_pins + i));
        }
        this->_activeNode->vcc = 0;
        this->_activeNode->gnd = 0;
        this->_state = MXCHARLIE_INACTIVE;
    }

    // Help from http://www.instructables.com/id/Charlieplexing-the-Arduino/
    MatrixCharlieplex::_upPin(uint8_t pin) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
        return true;
    }

    MatrixCharlieplex::_downPin(uint8_t pin) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        return true;
    }

    MatrixCharlieplex::_sinkPin(uint8_t pin) {
        pinMode(pin, INPUT);
        digitalWrite(pin, LOW);
        return true;
    }

    MatrixCharlieplex::_setNode(DiodeNode* pin, uint8_t state) {
        if (state) {
            switch (this->_activeNode->vcc) {
                case pin->vcc:
                    break;
                case pin->gnd:
                    _downPin(pin->gnd);
                    break;
                default:
                    if(this->_state)
                        _sinkPin(this->_activeNode->vcc);
                    break;
            };
            switch (this->_activeNode->gnd) {
                case pin->vcc:
                    _upPin(pin->vcc);
                    break;
                case pin->gnd:
                    break;
                default:
                    if(this->_state)
                        _sinkPin(this->_activeNode->gnd);
                    break;
            };
            this->_activeNode->vcc = pin->vcc;
            this->_activeNode->gnd = pin->gnd;
            this->_state = MXCHARLIE_ACTIVE;
            return true;
        } else { //To do
            uint8_t _chkMatch = 0;
            uint8_t _chkConflict = 0;
            uint8_t _chkClear = 0;
            switch (this->_activeNode->vcc) {
                case pin->vcc:
                    _chkMatch = (_chkMatch<<1)|1;
                    break;
                case pin->gnd:
                    _chkConflict = (_chkConflict<<1)|1;
                    break;
                default:
                    _chkClear = (_chkClear<<1)|1;
                    break;
            };
            switch (this->_activeNode->gnd) {
                case pin->vcc:
                    _chkConflict = (_chkConflict<<1)|1;
                    break;
                case pin->gnd:
                    _chkMatch = (_chkMatch<<1)|1;
                    break;
                default:
                    _chkClear = (_chkClear<<1)|1;
                    break;
            };
            if(0b00 & _chkClear){
                    _sinkPin(pin->vcc);
                    _sinkPin(pin->gnd);
            }
                    this->_activeNode->vcc = 0;
                    this->_activeNode->gnd = 0;
            return true;
        }
        return false;
    }

    MatrixCharlieplex::_getNode(uint8_t row, uint8_t col) {
        DiodeNode* node = new DiodeNode();
        return node;
    }
#pragma endregion Private Functions

#pragma region Public Functions

    MatrixCharlieplex::TurnOn(uint8_t row, uint8_t col) {
        return _setNode(_getNode(row, col), HIGH);
    }

    MatrixCharlieplex::TurnOff(uint8_t row, uint8_t col) {
        return _setNode(_getNode(row, col), LOW);
    }

    MatrixCharlieplex::Clear() {
        return _setNode(this->_activeNode, LOW);
    }

    MatrixCharlieplex::Reset() {
        if (_setNode(this->_activeNode, LOW))
            return _init(true);
        return false;
    }
#pragma endregion Public Functions
}