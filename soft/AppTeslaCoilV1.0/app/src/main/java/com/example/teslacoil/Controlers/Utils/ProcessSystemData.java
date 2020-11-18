package com.example.teslacoil.Controlers.Utils;

import android.util.Log;

import com.example.teslacoil.Controlers.Tesla;

import static com.example.teslacoil.Controlers.Tesla.LOW_POWER_PROTECT;
import static com.example.teslacoil.Controlers.Tesla.TERMO_PROTECT;
import static com.example.teslacoil.Controlers.Tesla.TESLA_STATUS;
import static com.example.teslacoil.Utils.Utils.Thermister;
import static com.example.teslacoil.View.Fragments.MoreFragment.SET_TEMPERATURE_AND_POWER_VALUE;

public class ProcessSystemData {
    private static ProcessSystemData processSystemData = new ProcessSystemData();

    public static ProcessSystemData getInstance() {
        return processSystemData;
    }

    private ProcessSystemData() {}

    public boolean isTermoProtect, isLowPower;
    public float valuePowerVoltage, valueTemperature;
    private boolean oldTermoProtect, oldLowPower;

    public static final byte RX_SYSTEM_DATA_KEY = 0x00;
    public static final byte RX_SYSTEM_DATA_MSK = (byte) (0x80 | (RX_SYSTEM_DATA_KEY << 5));

    public static final byte RX_IDENTIFICATION_KEY1 = RX_SYSTEM_DATA_MSK | 0x01;
    public static final byte RX_IDENTIFICATION_KEY2 = RX_SYSTEM_DATA_MSK | 0x02;
    public static final byte RX_SET_POWER = RX_SYSTEM_DATA_MSK | 0x06;
    public static final byte RX_SET_TEMP = RX_SYSTEM_DATA_MSK | 0x03;
    public static final byte RX_SET_TEMP_CPU = RX_SYSTEM_DATA_MSK | 0x07;


    public static final byte TX_SYSTEM_DATA_KEY = 0x00;
    private static final byte TX_SYSTEM_DATA_MSK = (byte) (0x80 | (TX_SYSTEM_DATA_KEY << 5));

    public static final byte TX_IDENTIFICATION_KEY1 = TX_SYSTEM_DATA_MSK | 0x01;
    public static final byte TX_IDENTIFICATION_KEY2 = TX_SYSTEM_DATA_MSK | 0x02;
    public static final byte TX_CHANGE_CONNECTION = TX_SYSTEM_DATA_MSK | 0x04;
    public static final byte TX_DISCONNECT = TX_SYSTEM_DATA_MSK | 0x05;


    private byte RX_IDENTIFICATION_KEY1_VAL = 0x45;
    private byte RX_IDENTIFICATION_KEY2_VAL = 0x76;

    private byte TX_IDENTIFICATION_KEY1_VAL = 0x32;
    private byte TX_IDENTIFICATION_KEY2_VAL = 0x64;
    public void process(byte buff[]) {
        switch (buff[0]) {
            default: break;

            case RX_IDENTIFICATION_KEY1: processIdentificationKey1(buff[1]); break;
            case RX_IDENTIFICATION_KEY2: processIdentificationKey2(buff[1]); break;
            case RX_SET_POWER: setPowerValue(buff); break;
            case RX_SET_TEMP: setTemperature(buff); break;
        }
    }

    private void processIdentificationKey1(byte key) {
        if(key == RX_IDENTIFICATION_KEY1_VAL) {
            IdentificationDevice.getInstance().setKey1();
        }
    }

    private void processIdentificationKey2(byte key) {
        if(key == RX_IDENTIFICATION_KEY2_VAL) {
            IdentificationDevice.getInstance().setKey2();
        }
    }

    private void setTemperature(byte data[]) {
        isTermoProtect = (data[3] > 0)? true : false;

        int valTemp = (data[1] << 7) | data[2];
        Log.d("Temp", String.valueOf(valTemp));
        valueTemperature = (float) Thermister(valTemp);

        if(oldTermoProtect != isTermoProtect) {
            oldTermoProtect = isTermoProtect;
            Tesla.getInstance().setMessage(TESLA_STATUS, TERMO_PROTECT, 0, isTermoProtect);
        }

        Tesla.getInstance().setMessage(TESLA_STATUS, SET_TEMPERATURE_AND_POWER_VALUE, 0);
    }

    private void setPowerValue(byte data[]) {
        isLowPower = (data[3]> 0)? true : false;

        int valuePower = (data[1] << 7) | data[2];
        valuePowerVoltage = ((float) valuePower) / 100;

        if(oldLowPower != isLowPower) {
            oldLowPower = isLowPower;
            Tesla.getInstance().setMessage(TESLA_STATUS, LOW_POWER_PROTECT, 0, isLowPower);
        }

        Tesla.getInstance().setMessage(TESLA_STATUS, SET_TEMPERATURE_AND_POWER_VALUE, 0);
    }

    public void TxIdentificationKey1() {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_IDENTIFICATION_KEY1, TX_IDENTIFICATION_KEY1_VAL, 0, 0});
    }

    public void TxIdentificationKey2() {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_IDENTIFICATION_KEY2, TX_IDENTIFICATION_KEY2_VAL, 0, 0});
    }

    public void TxChangeConnection() {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_CHANGE_CONNECTION, 0, 0, 0});
    }

    public void setTxDisconnect() {
        Tesla.getInstance().receiveTransmit.TransmitFrame(new byte[] {TX_DISCONNECT, 0, 0, 0});
    }
}
