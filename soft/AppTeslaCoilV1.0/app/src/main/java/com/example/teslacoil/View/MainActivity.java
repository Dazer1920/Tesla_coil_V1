package com.example.teslacoil.View;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;
import android.widget.Toast;

import com.example.teslacoil.Controlers.FragmentController;
import com.example.teslacoil.Controlers.InterrupterController;
import com.example.teslacoil.Controlers.MidiPlayerController;
import com.example.teslacoil.R;
import com.example.teslacoil.Controlers.Tesla;
import com.example.teslacoil.View.Fragments.AboutFragment;
import com.example.teslacoil.View.Fragments.InterrupterFragment;
import com.example.teslacoil.View.Fragments.MidiFragment;
import com.example.teslacoil.View.Fragments.MoreFragment;

import static com.example.teslacoil.Controlers.Bluetooth.BLUETOOTH_STATUS;
import static com.example.teslacoil.Controlers.Bluetooth.DEVICE_NOT_FOUND_1;
import static com.example.teslacoil.Controlers.Bluetooth.DEVICE_NOT_FOUND_2;
import static com.example.teslacoil.Controlers.Bluetooth.DEVICE_WENT_OUT_OF_RANGE;
import static com.example.teslacoil.Controlers.Bluetooth.DISCONNECTED_BY_DEVICE;
import static com.example.teslacoil.Controlers.Bluetooth.SET_ERROR;
import static com.example.teslacoil.Controlers.Bluetooth.STATUS_DISCONNECTED;
import static com.example.teslacoil.Controlers.FragmentController.ABOUT_FRAGMENT_POSITION;
import static com.example.teslacoil.Controlers.FragmentController.INTERRUPTER_FRAGMENT_POSITION;
import static com.example.teslacoil.Controlers.FragmentController.MIDI_FRAGMENT_POSITION;
import static com.example.teslacoil.Controlers.FragmentController.MORE_FRAGMENT_POSITION;
import static com.example.teslacoil.Controlers.Tesla.TESLA_STATUS;
import static com.example.teslacoil.View.Fragments.MidiFragment.EXTERNAL_REQUEST;
import static com.example.teslacoil.View.Fragments.MidiFragment.OPEN_FILE_ACTIV_CODE;
import static com.example.teslacoil.View.Fragments.MidiFragment.OPEN_FILE_RESULT;

public class MainActivity extends AppCompatActivity {
    private static MainActivity mainActivity;

    public static MainActivity getInstance() {return mainActivity;}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        mainActivity = this;

        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        getSupportActionBar().setHomeAsUpIndicator(R.drawable.ic_disconnect);

        @SuppressLint("HandlerLeak") Handler handler = new Handler() {
            @Override
            public void handleMessage(@NonNull Message msg) {
                switch (msg.what) {
                    default: break;
                    case BLUETOOTH_STATUS: processHandlerBluetooth(msg); break;
                    case TESLA_STATUS: processHandlerTesla(msg); break;
                }
            }
        };
        Tesla.getInstance().setHandler(handler);

        if(FragmentController.getInstance().getPosition() == 0) FragmentController.getInstance().openFragment(INTERRUPTER_FRAGMENT_POSITION);
        else FragmentController.getInstance().showFragment();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if(InterrupterController.getInstance().isEnable || MidiPlayerController.getInstance().isPlay())
            return super.onOptionsItemSelected(item);

        if(id == android.R.id.home) {
            Tesla.getInstance().DisconnectDevice(false);
        }

        switch (id) {
            default: break;

            case R.id.interrupterButton: FragmentController.getInstance().openFragment(INTERRUPTER_FRAGMENT_POSITION); break;
            case R.id.midiButton: FragmentController.getInstance().openFragment(MIDI_FRAGMENT_POSITION); break;
            case R.id.moreButton: FragmentController.getInstance().openFragment(MORE_FRAGMENT_POSITION); break;
            case R.id.aboutButton: FragmentController.getInstance().openFragment(ABOUT_FRAGMENT_POSITION); break;
        }

        return super.onOptionsItemSelected(item);
    }

    private void processHandlerTesla(Message msg) {
        switch (FragmentController.getInstance().getPosition()) {
            default: break;
            case INTERRUPTER_FRAGMENT_POSITION: InterrupterFragment.getInstance().setHandlerEvent(msg); break;
            case MIDI_FRAGMENT_POSITION: MidiFragment.getInstance().setHandlerEvent(msg); break;
            case MORE_FRAGMENT_POSITION: MoreFragment.getInstance().setHandlerEvent(msg); break;
            case ABOUT_FRAGMENT_POSITION: AboutFragment.getInstance().setHandlerEvent(msg); break;
        }
    }


    private void processHandlerBluetooth(Message msg) {
        switch (msg.arg1) {
            default: break;
            case STATUS_DISCONNECTED: Disconnected((boolean) msg.obj); break;
            case SET_ERROR: setError(msg.arg2); break;
        }
    }

    private void Disconnected(boolean isError) {
        stopController();

        FragmentController.getInstance().setPosition(INTERRUPTER_FRAGMENT_POSITION);
        if(!isError) Toast.makeText(this, getString(R.string.disconnected), Toast.LENGTH_LONG).show();
        Intent intent = new Intent(MainActivity.this, StartActivity.class);
        startActivity(intent);
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
        }
    }

    private void stopController() {
        switch (FragmentController.getInstance().getPosition()) {
            default: break;
            case INTERRUPTER_FRAGMENT_POSITION: InterrupterFragment.getInstance().offController(); break;
            case MIDI_FRAGMENT_POSITION: MidiPlayerController.getInstance().setPause(); break;
            case MORE_FRAGMENT_POSITION:  break;
            case ABOUT_FRAGMENT_POSITION:  break;
        }
    }

    @Override
    protected void onDestroy() {
        stopController();
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        stopController();
        super.onPause();
    }

    @Override
    protected void onStop() {
        stopController();
        super.onStop();
    }

    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == EXTERNAL_REQUEST) {
            if (grantResults.length > 0 && grantResults[1] == PackageManager.PERMISSION_GRANTED) {
                Intent intent = new Intent(MainActivity.this, FileManager.class);
                startActivityForResult(intent, OPEN_FILE_ACTIV_CODE);
            }
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == OPEN_FILE_ACTIV_CODE) {
            String message = data.getStringExtra(OPEN_FILE_RESULT);
            if (message != null) {
                Tesla.getInstance().setMessage(TESLA_STATUS, OPEN_FILE_ACTIV_CODE, 0, message);
            }
        }
    }
}
