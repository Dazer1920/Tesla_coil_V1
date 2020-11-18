package com.example.teslacoil.View.Fragments;

import android.annotation.SuppressLint;
import android.app.Fragment;
import android.os.Bundle;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.example.teslacoil.Controlers.InterrupterController;
import com.example.teslacoil.R;

import static com.example.teslacoil.Controlers.Tesla.LOW_POWER_PROTECT;
import static com.example.teslacoil.Controlers.Tesla.TERMO_PROTECT;
import static com.example.teslacoil.Utils.Pref.saveData;

public class InterrupterFragment extends Fragment {
    private static InterrupterFragment interrupterFragment;
    private Button PulseButton;
    private ToggleButton EnbaleButton;
    private SeekBar[] seekBars = new SeekBar[4];
    private TextView[] MaxTextViews = new TextView[4];
    private TextView[] MinTextViews = new TextView[4];
    private TextView[] valText = new TextView[4];

    public static InterrupterFragment getInstance() {
        return interrupterFragment;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_interrupter, null);
        interrupterFragment = this;

        PulseButton = (Button) view.findViewById(R.id.PulseButton);
        EnbaleButton = (ToggleButton) view.findViewById(R.id.EnableButton);

        seekBars[0] = (SeekBar) view.findViewById(R.id.sliderVal1);
        seekBars[1] = (SeekBar) view.findViewById(R.id.sliderVal2);
        seekBars[2] = (SeekBar) view.findViewById(R.id.sliderVal3);
        seekBars[3] = (SeekBar) view.findViewById(R.id.sliderVal4);

        MaxTextViews[0] = (TextView) view.findViewById(R.id.maxValText1);
        MaxTextViews[1] = (TextView) view.findViewById(R.id.maxValText2);
        MaxTextViews[2] = (TextView) view.findViewById(R.id.maxValText3);
        MaxTextViews[3] = (TextView) view.findViewById(R.id.maxValText4);

        MinTextViews[0] = (TextView) view.findViewById(R.id.minValText1);
        MinTextViews[1] = (TextView) view.findViewById(R.id.minValText2);
        MinTextViews[2] = (TextView) view.findViewById(R.id.minValText3);
        MinTextViews[3] = (TextView) view.findViewById(R.id.minValText4);

        valText[0] = (TextView) view.findViewById(R.id.valueText1);
        valText[1] = (TextView) view.findViewById(R.id.valueText2);
        valText[2] = (TextView) view.findViewById(R.id.valueText3);
        valText[3] = (TextView) view.findViewById(R.id.valueText4);

        for(int i = 0; i < seekBars.length; i++) {
            seekBars[i].setMax(InterrupterController.getInstance().max[i] - InterrupterController.getInstance().min[i]);
            MaxTextViews[i].setText(String.valueOf(InterrupterController.getInstance().max[i]));
            MinTextViews[i].setText(String.valueOf(InterrupterController.getInstance().min[i]));
        }

        PulseButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                InterrupterController.getInstance().setPulse();
            }
        });

        EnbaleButton.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                InterrupterController.getInstance().setEnableInterrupter(isChecked);
            }
        });

        InterrupterController.getInstance().setStateInterrupterController(stateInterrupterController);
        InterrupterController.getInstance().startController();

        return view;
    }

    private SeekBar.OnSeekBarChangeListener progress = new SeekBar.OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            for(int i = 0; i < seekBars.length; i++) {
                if(seekBars[i].getId() == seekBar.getId()) {
                    InterrupterController.getInstance().setValueInterrupter(progress, i);
                    break;
                }
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {

        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            for(int i = 0; i < seekBars.length; i++) {
                if(seekBars[i].getId() == seekBar.getId()) {
                    saveData("KeyValSeecbarInrer" + i, seekBar.getProgress() + InterrupterController.getInstance().min[i]);
                    break;
                }
            }
        }
    };

    public void setHandlerEvent(Message msg) {
        if(!isAdded()) return;

        switch (msg.arg1) {
            default: break;

            case TERMO_PROTECT: termoProtect((boolean) msg.obj); break;
            case LOW_POWER_PROTECT: lowPowerProtect((boolean) msg.obj); break;
        }
    }

    private void termoProtect(boolean isTermoProtect) {
        if(isTermoProtect) Toast.makeText(getActivity(), getString(R.string.high_temp_toast), Toast.LENGTH_LONG).show();
        InterrupterController.getInstance().setProtect();
    }

    private void lowPowerProtect(boolean isLowPower) {
        if(isLowPower) Toast.makeText(getActivity(), getString(R.string.low_power_toast), Toast.LENGTH_LONG).show();
        InterrupterController.getInstance().setProtect();
    }

    private InterrupterController.StateInterrupterController stateInterrupterController = new InterrupterController.StateInterrupterController() {
        @Override
        public void onOffInterrupter() {
            if(EnbaleButton.isChecked()) EnbaleButton.setChecked(false);
            PulseButton.setEnabled(true);
        }

        @Override
        public void onOnInterrupter() {
            if(!EnbaleButton.isChecked()) EnbaleButton.setChecked(true);
            PulseButton.setEnabled(false);
        }

        @SuppressLint("SetTextI18n")
        @Override
        public void setValue(int value, int code) {
            String[] mes = new String[] {
                    getString(R.string.value_power),
                    getString(R.string.value_freq),
                    getString(R.string.value_mod_duty),
                    getString(R.string.value_mod_freq)
            };

            String[] uint = new String[] {
                    getString(R.string.value_util_power),
                    getString(R.string.value_util_freq),
                    getString(R.string.value_util_mod_duty),
                    getString(R.string.value_util_mod_freq)
            };

            for (int i = 0; i < valText.length; i++) {
                if(i == code) {
                    valText[i].setText(mes[i] + " " + value + uint[i]);
                    break;
                }
            }
        }

        @Override
        public void onProtectOn() {
            EnbaleButton.setChecked(false);
            EnbaleButton.setEnabled(false);
            PulseButton.setEnabled(false);
        }

        @Override
        public void onProtectOff() {
            EnbaleButton.setChecked(false);
            EnbaleButton.setEnabled(true);
            PulseButton.setEnabled(true);
        }

        @Override
        public void endInit() {
            for(int i = 0; i < seekBars.length; i++) {
                seekBars[i].setOnSeekBarChangeListener(progress);
            }
        }

        @Override
        public void initValues(int value, int code) {
            seekBars[code].setProgress(value);
        }
    };

    public void offController() {
        InterrupterController.getInstance().setEnableInterrupter(false);
    }
}