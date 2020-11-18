package com.example.teslacoil.View;

import android.Manifest;
import android.app.AlertDialog;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.example.teslacoil.R;
import com.example.teslacoil.Controlers.Tesla;

import java.util.Set;

import static com.example.teslacoil.Controlers.Bluetooth.BLUETOOTH_STATUS;
import static com.example.teslacoil.Controlers.Bluetooth.STATUS_START_CONNECT;
import static com.example.teslacoil.Utils.Utils.setListViewHeightBasedOnChildren;

public class ConnectActivity extends AppCompatActivity {
    private static ConnectActivity connectActivity;

    public static ConnectActivity getInstance() {return connectActivity;}

    private Button StartScanButton;
    private ProgressBar progressScaner;
    private ListView scanDevicesList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_connect);
        connectActivity = this;

        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        setupScaner();
        setupPairedList();
        checkBTPermissions();
        checkLocationServiceEnabledDialog();
    }

    ArrayAdapter<String> scanDevicesArray;
    int countDevises = 0;

    private void setupScaner() {
        scanDevicesArray = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1);

        progressScaner = (ProgressBar) findViewById(R.id.progressBar);
        StartScanButton = (Button) findViewById(R.id.scanButton);

        scanDevicesList = (ListView) findViewById(R.id.listDevices);
        scanDevicesList.setAdapter(scanDevicesArray);
        scanDevicesList.setTranscriptMode(0);
        setOnClickItem(scanDevicesList);

        StartScanButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Tesla.getInstance().startLeScan(ConnectActivity.this);
            }
        });
    }

    public void OnStartScan() {
        StartScanButton.setEnabled(false);
        countDevises = 0;
        progressScaner.setVisibility(View.VISIBLE);
        scanDevicesArray.clear();
    }

    public void OnStopScan() {
        progressScaner.setVisibility(View.INVISIBLE);
        StartScanButton.setEnabled(true);
    }

    public void OnNewDevice(BluetoothDevice device) {
        for(int i = 0; i < scanDevicesArray.getCount(); i++) {
            String a = scanDevicesArray.getItem(i).substring(scanDevicesArray.getItem(i).length() - 17);
            if(a.equals(device.getAddress())) {
                return;
            }
        }

        countDevises++;
        scanDevicesArray.add(countDevises + ". " + device.getName() + '\n' + "    " + device.getAddress());
        setListViewHeightBasedOnChildren(scanDevicesList);
    }

    private void setupPairedList() {
        ListView pairedDevises = (ListView) findViewById(R.id.parDevices);
        int countPairedDevice = 0;

        ArrayAdapter<String> pairedDevicesAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1);
        Set<BluetoothDevice> pairedDevices = Tesla.getInstance().bluetoothAdapter.getBondedDevices();

        if (pairedDevices.size() > 0) {
            for (BluetoothDevice device : pairedDevices) {
                countPairedDevice++;
                pairedDevicesAdapter.add(countPairedDevice + ". " + device.getName() + '\n' + "    " + device.getAddress());
            }
        }

        if (pairedDevicesAdapter.getCount() == 0) {
            pairedDevicesAdapter.add(getString(R.string.no_paired_devices));
        }

        pairedDevises.setAdapter(pairedDevicesAdapter);
        setListViewHeightBasedOnChildren(pairedDevises);
        setOnClickItem(pairedDevises);
    }

    private void setOnClickItem(ListView listView) {
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                if(Tesla.getInstance().bluetoothAdapter.isEnabled()) {
                    String device = ((TextView) view).getText().toString();
                    if(device.equals(getString(R.string.no_paired_devices))) return;

                    String address = device.substring(device.length() - 17);
                    BluetoothDevice bluetoothDevice = Tesla.getInstance().bluetoothAdapter.getRemoteDevice(address);

                    if(bluetoothDevice.getType() == BluetoothDevice.DEVICE_TYPE_CLASSIC) {
                        Tesla.getInstance().setMessage(BLUETOOTH_STATUS, STATUS_START_CONNECT, 0, bluetoothDevice);
                        finish();
                    }
                }
            }
        });
    }

    public void checkBTPermissions() {
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP_MR1) {
            int permissionCheck = this.checkSelfPermission("Manifest.permission.ACCESS_FINE_LOCATION");
            permissionCheck += this.checkSelfPermission("Manifest.permission.ACCESS_COARSE_LOCATION");
            if (permissionCheck != 0) {
                this.requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION,
                        Manifest.permission.ACCESS_COARSE_LOCATION}, 0);
            }
        } else {
            Tesla.getInstance().startLeScan(this);
        }
    }

    private LocationManager locationManager;
    public static boolean geolocationEnabled = false;

    private boolean checkLocationServiceEnabledDialog() {
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.P) {
            locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
            try {
                geolocationEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
            } catch (Exception ex) {
            }

            return buildAlertMessageNoLocationService(geolocationEnabled);
        }

        return false;
    }

    private boolean buildAlertMessageNoLocationService(boolean network_enabled) {
        String msg = !network_enabled ? getString(R.string.geolocation_msg) : null;

        if (msg != null) {
            OnStopScan();

            final AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setCancelable(false)
                    .setMessage(msg)
                    .setPositiveButton(getString(R.string.geolocation_enable), new DialogInterface.OnClickListener() {
                        public void onClick(@SuppressWarnings("unused") final DialogInterface dialog, @SuppressWarnings("unused") final int id) {
                            startActivity(new Intent(android.provider.Settings.ACTION_LOCATION_SOURCE_SETTINGS));
                        }
                    })
                    .setNegativeButton(getString(R.string.geolocation_no), new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {

                }
            });
            final AlertDialog alert = builder.create();
            alert.show();
            return true;
        }
        return false;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if(id == android.R.id.home) finish();

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onStop() {
        Tesla.getInstance().unsignedReciver(this);
        super.onStop();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == 0) {
            if (grantResults.length > 0 && grantResults[1] == PackageManager.PERMISSION_GRANTED) {
                Tesla.getInstance().startLeScan(this);
            }
        }
    }

    @Override
    public void finish() {
        if(Tesla.getInstance().bluetoothAdapter.isDiscovering()) Tesla.getInstance().stopLeScan();
        super.finish();
    }
}
