package com.example.teslacoil.View;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.example.teslacoil.R;
import com.example.teslacoil.Controlers.Tesla;

import static com.example.teslacoil.Controlers.Bluetooth.BLUETOOTH_STATUS;
import static com.example.teslacoil.Controlers.Bluetooth.DEVICE_NOT_FOUND_1;
import static com.example.teslacoil.Controlers.Bluetooth.DEVICE_NOT_FOUND_2;
import static com.example.teslacoil.Controlers.Bluetooth.DEVICE_WENT_OUT_OF_RANGE;
import static com.example.teslacoil.Controlers.Bluetooth.DISCONNECTED_BY_DEVICE;
import static com.example.teslacoil.Controlers.Bluetooth.SET_ERROR;
import static com.example.teslacoil.Controlers.Bluetooth.STATUS_CONNECTED;
import static com.example.teslacoil.Controlers.Bluetooth.STATUS_DISCONNECTED;
import static com.example.teslacoil.Controlers.Bluetooth.STATUS_NEW_DEVICE;
import static com.example.teslacoil.Controlers.Bluetooth.STATUS_START_CONNECT;
import static com.example.teslacoil.Controlers.Bluetooth.STATUS_START_CSAN;
import static com.example.teslacoil.Controlers.Bluetooth.STATUS_STOP_SCAN;
import static com.example.teslacoil.Controlers.Utils.IdentificationDevice.IDENTIFICATION_DEVICE_FAILED;
import static com.example.teslacoil.Controlers.Utils.IdentificationDevice.STATUS_DEVICE_IDENTIFICATED;

public class StartActivity extends Activity {
    private ProgressDialog progressDialog;
    private static final int BLUETOOTH_ENABLE_CODE = 232;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);

        if(Tesla.getInstance().isConnectedDevice()) {
            OpenMainScreen();
            finish();
        }

        final Button ConnectButt = (Button) findViewById(R.id.ConnectButton);
        ConnectButt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(Tesla.getInstance().bluetoothAdapter.isEnabled()) {
                    OpenConnectScreen();
                } else {
                    Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(intent, BLUETOOTH_ENABLE_CODE);
                }
            }
        });

        @SuppressLint("HandlerLeak") Handler handler = new Handler() {
            @Override
            public void handleMessage(@NonNull Message msg) {
                if(msg.what != BLUETOOTH_STATUS) return;

                switch (msg.arg1) {
                    default: break;

                    case STATUS_START_CONNECT: StartConnect((BluetoothDevice) msg.obj); break;
                    case STATUS_CONNECTED: Connected(); break;
                    case STATUS_DISCONNECTED: Disconnected((boolean) msg.obj); break;
                    case STATUS_DEVICE_IDENTIFICATED: Identificated(); break;

                    case SET_ERROR: setError(msg.arg2); break;

                    case STATUS_START_CSAN: ConnectActivity.getInstance().OnStartScan(); break;
                    case STATUS_NEW_DEVICE: ConnectActivity.getInstance().OnNewDevice((BluetoothDevice) msg.obj); break;
                    case STATUS_STOP_SCAN: ConnectActivity.getInstance().OnStopScan(); break;
                }
            }
        };

        Tesla.getInstance().setHandler(handler);
        Tesla.getInstance().setBluetoothAdapter(BluetoothAdapter.getDefaultAdapter());
    }

    private void StartConnect(BluetoothDevice bluetoothDevice) {
        progressDialog = ProgressDialog.show(this, getString(R.string.connect_to_device_progress) + " "
                + bluetoothDevice.getName(), getString(R.string.connecting_progress));

        Tesla.getInstance().Connect(bluetoothDevice);
    }

    private void Connected() {
        if(progressDialog != null) progressDialog.setMessage(getString(R.string.identification_progress));
    }

    private void Disconnected(boolean isError) {
        if(progressDialog != null && progressDialog.isShowing()) progressDialog.dismiss();
        if(!isError) Toast.makeText(this, getString(R.string.disconnected), Toast.LENGTH_LONG).show();

    }

    private void Identificated() {
        if(progressDialog != null) progressDialog.dismiss();
        Toast.makeText(this, getString(R.string.connected), Toast.LENGTH_LONG).show();
        OpenMainScreen();
        finish();
    }

    public void setError(int codeError) {
        switch (codeError) {
            default: break;

            case DEVICE_NOT_FOUND_1:
            case DEVICE_NOT_FOUND_2:
                Toast.makeText(this, getString(R.string.error_device_not_found), Toast.LENGTH_LONG).show(); break;

            case DISCONNECTED_BY_DEVICE: Toast.makeText(this, getString(R.string.error_disconnected_by_device), Toast.LENGTH_LONG).show(); break;
            case DEVICE_WENT_OUT_OF_RANGE: Toast.makeText(this, getString(R.string.error_device_went_out_of_range), Toast.LENGTH_LONG).show(); break;
            case IDENTIFICATION_DEVICE_FAILED: Toast.makeText(this, getString(R.string.error_identification_fail), Toast.LENGTH_LONG).show(); break;
        }
    }

    private void OpenConnectScreen() {
        Intent intent = new Intent(StartActivity.this, ConnectActivity.class);
        startActivity(intent);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if(requestCode == BLUETOOTH_ENABLE_CODE) {
            if(resultCode == Activity.RESULT_OK) {
                OpenConnectScreen();
            }
        }
    }


    private void OpenMainScreen() {
        Intent intent = new Intent(StartActivity.this, MainActivity.class);
        startActivity(intent);
    }
}
