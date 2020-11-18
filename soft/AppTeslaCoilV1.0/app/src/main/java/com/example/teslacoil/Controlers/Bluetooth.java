package com.example.teslacoil.Controlers;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;

import com.example.teslacoil.Controlers.Utils.IdentificationDevice;
import com.example.teslacoil.Controlers.Utils.ProcessSystemData;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import static com.example.teslacoil.Controlers.Player.RX_MIDI_PLAYER_KEY;
import static com.example.teslacoil.Controlers.Utils.ProcessSystemData.RX_SYSTEM_DATA_KEY;
import static com.example.teslacoil.Controlers.Utils.ProcessSystemData.RX_SYSTEM_DATA_MSK;

public class Bluetooth {
    public static final int BLUETOOTH_STATUS = 1;

    public static final int STATUS_START_CONNECT = 0;
    public static final int STATUS_CONNECTED = 1;
    public static final int STATUS_DISCONNECTED = 2;

    public static final int STATUS_START_CSAN = 4;
    public static final int STATUS_NEW_DEVICE = 5;
    public static final int STATUS_STOP_SCAN = 6;

    public static final int SET_ERROR = 3;

    public static final int DEVICE_NOT_FOUND_1 = 133;
    public static final int DEVICE_NOT_FOUND_2 = 62;
    public static final int DISCONNECTED_BY_DEVICE = 19;
    public static final int DEVICE_WENT_OUT_OF_RANGE = 8;

    public BluetoothAdapter bluetoothAdapter;
    public BluetoothDevice currentDevice;
    private BluetoothSocket clientSocket;
    private InputStream InStrem;
    private OutputStream OutStream;
    public ReceiveTransmit receiveTransmit;

    public boolean isIdentificatedDevice = false;

    public void setBluetoothAdapter(BluetoothAdapter bluetoothAdapter) {
        this.bluetoothAdapter = bluetoothAdapter;
    }

    private BroadcastReceiver receiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action))
                Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_START_CSAN, 0);

            else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action))
                Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_STOP_SCAN, 0);

            else if (BluetoothDevice.ACTION_FOUND.equals(action))
                Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_NEW_DEVICE, 0, intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE));
        }
    };

    public void startLeScan(Context context) {
        if(bluetoothAdapter.isDiscovering()) return;

        IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        filter.addAction(BluetoothDevice.ACTION_FOUND);
        context.registerReceiver(receiver, filter);

        bluetoothAdapter.startDiscovery();
    }

    public void unsignedReciver(Context context) {
        if(receiver != null) {
            try{
                context.unregisterReceiver(receiver);
            }catch (Exception e){
                e.printStackTrace();
            }
        }
    }

    public void stopLeScan() {
        bluetoothAdapter.cancelDiscovery();
    }

    public synchronized boolean isConnected() {
        return clientSocket != null && clientSocket.isConnected();
    }

    public synchronized void writeData(byte[] data) {
        if(!isConnected()) return;

        try {
            OutStream.write(data);
        } catch (IOException e) {
            e.printStackTrace();
            InStrem = null;
            OutStream = null;
            currentDevice = null;
            isIdentificatedDevice = false;
            Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_DISCONNECTED, 0, true);
            Tesla.getInstance().setMessage(BLUETOOTH_STATUS, SET_ERROR, DEVICE_WENT_OUT_OF_RANGE);
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }

    public void Connect(final BluetoothDevice device) {
        currentDevice = device;

        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    clientSocket = currentDevice.createInsecureRfcommSocketToServiceRecord(
                            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
                } catch (IOException e) {
                    e.printStackTrace();

                    Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_DISCONNECTED, 0, true);
                    Tesla.getInstance().setMessage(BLUETOOTH_STATUS, SET_ERROR, DEVICE_NOT_FOUND_1);
                }

                try {
                    clientSocket.connect();
                    InStrem = clientSocket.getInputStream();
                    OutStream = clientSocket.getOutputStream();
                    receiveTransmit = new ReceiveTransmit();
                    startReceiveStream();

                    Tesla.getInstance().ConnectedToDevice();

                    IdentificationDevice.getInstance().startIdentification();
                } catch (IOException e) {
                    e.printStackTrace();

                    Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_DISCONNECTED, 0, true);
                    Tesla.getInstance().setMessage(BLUETOOTH_STATUS, SET_ERROR, DEVICE_NOT_FOUND_1, null);
                }
            }
        }).start();
    }

    public void disconnect(boolean isError) {
        try {
            clientSocket.close();
            InStrem = null;
            OutStream = null;
            currentDevice = null;
            isIdentificatedDevice = false;
            Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_DISCONNECTED, 0, isError);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void startReceiveStream() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                while (isConnected()) {
                    try {
                        if(InStrem.available() > 0) {
                            byte data = (byte) InStrem.read();
                            receiveTransmit.reabBytes(data);
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    } catch (NullPointerException e) {
                        e.printStackTrace();
                        break;
                    }
                }
            }
        }).start();
    }

    public class ReceiveTransmit {
        final static int SIZE_RX_BUFF = 4;

        public synchronized void TransmitFrame(byte[] data) {
            int sum = 0;
            byte[] trans = new byte[5];

            for(int i = 0; i < trans.length - 1; i++) {
                trans[i] = data[i];
                sum += data[i];
            }

            trans[4] = (byte) ((~(sum + 1)) & 0x007f);

            writeData(trans);
        }

        private byte[] buffData = new byte[SIZE_RX_BUFF];
        private byte sum, countBuff;

        public void reabBytes(byte data) {
            if((data & 0x80) > 0) {
                sum = 0;
                countBuff = 0;
                buffData = new byte[SIZE_RX_BUFF];
            } else {
                if(countBuff == SIZE_RX_BUFF && data == ((~(sum + 1)) & 0x007f)) {
                    switch ((buffData[0] & 0x60) >> 5) {
                        default:
                            break;

                        case RX_SYSTEM_DATA_KEY:
                            ProcessSystemData.getInstance().process(buffData);
                            break;
                        case RX_MIDI_PLAYER_KEY:
                            MidiPlayerController.getInstance().player.readMidiData(buffData);
                            break;
                    }
                }
            }

            if(countBuff < SIZE_RX_BUFF) {
                buffData[countBuff] = data;
                countBuff++;
                sum += data;
            }
        }
    }
}
