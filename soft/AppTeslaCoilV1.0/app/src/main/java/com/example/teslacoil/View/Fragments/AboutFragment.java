package com.example.teslacoil.View.Fragments;

import android.annotation.SuppressLint;
import android.app.Fragment;
import android.os.Bundle;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.example.teslacoil.R;

public class AboutFragment extends Fragment {
    private static AboutFragment aboutFragment;

    public static AboutFragment getInstance() {
        return aboutFragment;
    }

    @SuppressLint("SetTextI18n")
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_about, null);
        aboutFragment = this;

        TextView aboutText = (TextView) view.findViewById(R.id.aboutText);
        aboutText.setText(R.string.about_text);

        return view;
    }

    public void setHandlerEvent(Message msg) {

    }
}