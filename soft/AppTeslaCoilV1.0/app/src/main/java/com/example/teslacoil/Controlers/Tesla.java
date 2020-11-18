package com.example.teslacoil.Controlers;

import android.os.Handler;
import android.os.Message;

import com.example.teslacoil.Controlers.Utils.ProcessSystemData;

import static com.example.teslacoil.Controlers.Utils.IdentificationDevice.STATUS_DEVICE_IDENTIFICATED;

public class Tesla extends Bluetooth {
    public static final int TESLA_STATUS = 2;

    public static final int TERMO_PROTECT = 3;
    public static final int LOW_POWER_PROTECT = 6;

    private static Tesla tesla = new Tesla();
    private Handler currentHandler;

    private Handler handlerConnection = new Handler();

    private static final long SCAN_PERIOD = 1000;

    public static Tesla getInstance() {
        return tesla;
    }

    private Tesla() {}

    public void setHandler(Handler h) {
        currentHandler = h;
    }

    public void setMessage(int what, Object obj) {
        Message msg = currentHandler.obtainMessage(what, obj);
        currentHandler.sendMessage(msg);
    }

    public void setMessage(int what) {
        Message msg = currentHandler.obtainMessage(what);
        currentHandler.sendMessage(msg);
    }

    public void setMessage(int what, int arg1, int arg2) {
        Message msg = currentHandler.obtainMessage(what, arg1, arg2);
        currentHandler.sendMessage(msg);
    }

    public void setMessage(int what, int arg1, int arg2, Object obg) {
        Message msg = currentHandler.obtainMessage(what, arg1, arg2, obg);
        currentHandler.sendMessage(msg);
    }

    public void ConnectedToDevice() {
        Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_CONNECTED, 0);
    }

    public void DeviceIdentificated() {
        Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_DEVICE_IDENTIFICATED, 0);
        startScanValuesDevice();
    }

    public void DisconnectDevice(boolean isError) {
        isIdentificatedDevice = false;
        if(!isError) ProcessSystemData.getInstance().setTxDisconnect();
        disconnect(isError);
    }

    public boolean isConnectedDevice() {
        return isConnected() && isIdentificatedDevice;
    }

    private void startScanValuesDevice() {
        handlerConnection.postDelayed(runnableConnection, SCAN_PERIOD);
    }

    public void restartScanValueDevice() {
        handlerConnection.removeCallbacks(runnableConnection);
        startScanValuesDevice();
    }

    private Runnable runnableConnection = new Runnable() {
        @Override
        public void run() {
            if(isConnectedDevice()) {
                ProcessSystemData.getInstance().TxChangeConnection();
                startScanValuesDevice();
            }
        }
    };
}
