package com.example.teslacoil.View.Fragments;

import android.annotation.SuppressLint;
import android.app.Fragment;
import android.os.Bundle;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import androidx.core.content.ContextCompat;

import com.example.teslacoil.Controlers.Tesla;
import com.example.teslacoil.Controlers.Utils.ProcessSystemData;
import com.example.teslacoil.R;

import static com.example.teslacoil.Controlers.Tesla.LOW_POWER_PROTECT;
import static com.example.teslacoil.Controlers.Tesla.TERMO_PROTECT;

public class MoreFragment extends Fragment {
    private static MoreFragment moreFragment;

    public static MoreFragment getInstance() {
        return moreFragment;
    }

    public static final int SET_TEMPERATURE_AND_POWER_VALUE = 1;

    private TextView temperatuteText, powerVoltText;

    @SuppressLint("SetTextI18n")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_more, null);
        moreFragment = this;

        TextView deviceText = (TextView) view.findViewById(R.id.currentDevice_text);
        deviceText.setText(getString(R.string.connected_device) + "\n\n" + Tesla.getInstance().currentDevice.getName() +
                '\n' + Tesla.getInstance().currentDevice.getAddress());

        temperatuteText = (TextView) view.findViewById(R.id.temperature_text);
        powerVoltText = (TextView) view.findViewById(R.id.volt_power_text);

        setSetTemperatureAndPowerValue();

        return view;
    }

    @SuppressLint({"DefaultLocale", "SetTextI18n"})
    public void setTemperatureText(double value, boolean termoprotect) {
        temperatuteText.setText(getString(R.string.temperature_trans) + String.format(" %.2f", value) + "°C");

        temperatuteText.setTextColor(ContextCompat.getColor(getActivity(),
                (termoprotect)? R.color.colorTempTextOn : R.color.colorTempTextOff));
    }

    public void setPowerText(float value, boolean lowPower) {
        powerVoltText.setText(getString(R.string.power_voltage) + String.format(" %.2f", value) + "V");

        powerVoltText.setTextColor(ContextCompat.getColor(getActivity(),
                (lowPower)? R.color.colorTempTextOn : R.color.colorTempTextOff));
    }

    public void setHandlerEvent(Message msg) {
        if(!isAdded()) return;

        switch (msg.arg1) {
            default: break;

            case SET_TEMPERATURE_AND_POWER_VALUE: setSetTemperatureAndPowerValue(); break;
            case TERMO_PROTECT: termoProtect((boolean) msg.obj); break;
            case LOW_POWER_PROTECT: lowPowerProtect((boolean) msg.obj); break;
        }
    }

    private void termoProtect(boolean isTermoProtect) {
        if(isTermoProtect) {
            Toast.makeText(getActivity(), getString(R.string.high_temp_toast), Toast.LENGTH_LONG).show();
        }
    }

    private void lowPowerProtect(boolean isLowPower) {
        if(isLowPower) {
            Toast.makeText(getActivity(), getString(R.string.low_power_toast), Toast.LENGTH_LONG).show();
        }
    }

    private void setSetTemperatureAndPowerValue() {
        setTemperatureText(ProcessSystemData.getInstance().valueTemperature, ProcessSystemData.getInstance().isTermoProtect);
        setPowerText(ProcessSystemData.getInstance().valuePowerVoltage, ProcessSystemData.getInstance().isLowPower);
    }
}
