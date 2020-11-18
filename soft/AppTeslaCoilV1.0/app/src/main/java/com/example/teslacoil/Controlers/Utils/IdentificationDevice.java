package com.example.teslacoil.Controlers.Utils;

import android.os.SystemClock;

import com.example.teslacoil.Controlers.Tesla;

import static com.example.teslacoil.Controlers.Bluetooth.BLUETOOTH_STATUS;
import static com.example.teslacoil.Controlers.Bluetooth.SET_ERROR;

public class IdentificationDevice {
    private static IdentificationDevice identificationDevice = new IdentificationDevice();

    public static IdentificationDevice getInstance() {
        return identificationDevice;
    }

    private IdentificationDevice() {}

    public static final int STATUS_DEVICE_IDENTIFICATED = 76;
    public static final int IDENTIFICATION_DEVICE_FAILED = 43;

    private boolean Key1 = false, Key2 = false;
    private int countRequests = 0, countKey = 0;
    private long mLastClickTime;

    public void startIdentification() {
        Key1 = false;
        Key2 = false;
        countRequests = 0;
        countKey = 0;

        long currentClickTime = SystemClock.uptimeMillis() + 1000;
        long elapsedTime = currentClickTime;

        while (true) {
            if(countKey == 1 && Key2 && !Key1) {
                failedIdentification();
                break;
            }

            if(countKey == 2 && Key2 && Key1) {
                Tesla.getInstance().isIdentificatedDevice = true;
                Tesla.getInstance().DeviceIdentificated();
                break;
            }

            currentClickTime = SystemClock.uptimeMillis();
            elapsedTime = currentClickTime - mLastClickTime;

            if(elapsedTime >= 1000) {
                mLastClickTime = currentClickTime;

                if(countRequests >=  2) {
                    failedIdentification();
                    break;
                }

                if(countKey == 0 && !Key1 && !Key2) ProcessSystemData.getInstance().TxIdentificationKey1();
                if(countKey == 1 && Key1 && !Key2) ProcessSystemData.getInstance().TxIdentificationKey2();
                countRequests++;
            }
        }
    }

    private void failedIdentification() {
        Tesla.getInstance().DisconnectDevice(true);
        Tesla.getInstance().setMessage(BLUETOOTH_STATUS, SET_ERROR, IDENTIFICATION_DEVICE_FAILED);
    }

    public void setKey1() {
        Key1 = true;
        countRequests = 0;
        countKey = 1;
        if(Key1 && !Key2) ProcessSystemData.getInstance().TxIdentificationKey2();
    }

    public void setKey2() {
        Key2 = true;
        countRequests = 0;
        countKey = 2;
    }
}
