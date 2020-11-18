package com.example.teslacoil.Controlers;

import android.app.Fragment;
import android.app.FragmentTransaction;

import com.example.teslacoil.R;
import com.example.teslacoil.View.Fragments.AboutFragment;
import com.example.teslacoil.View.Fragments.InterrupterFragment;
import com.example.teslacoil.View.Fragments.MidiFragment;
import com.example.teslacoil.View.Fragments.MoreFragment;
import com.example.teslacoil.View.MainActivity;

public class FragmentController {
    private static FragmentController fragmentController = new FragmentController();

    public static FragmentController getInstance() {return fragmentController;}

    public static final int INTERRUPTER_FRAGMENT_POSITION = 1;
    public static final int MIDI_FRAGMENT_POSITION = 2;
    public static final int MORE_FRAGMENT_POSITION = 3;
    public static final int ABOUT_FRAGMENT_POSITION = 4;

    private int position;

    private FragmentController() {}

    public int getPosition() {
        return position;
    }

    public void setPosition(int p) {
        position = p;
    }

    public void openFragment(int p) {
        if(p == 0 || this.position == p) {
            return;
        }

        Fragment[] fragments = {new InterrupterFragment(), new MidiFragment(), new MoreFragment(), new AboutFragment()};
        FragmentTransaction fragmentTransaction = MainActivity.getInstance().getFragmentManager().beginTransaction();
        for (Fragment f : fragments) {
            fragmentTransaction.remove(f);
        }

        position = p;

        fragmentTransaction.replace(R.id.mainContainer, fragments[position - 1]);
        fragmentTransaction.commit();
    }

    public void showFragment(int p) {
        if(p == 0) {
            return;
        }

        Fragment[] fragments = {new InterrupterFragment(), new MidiFragment(), new MoreFragment(), new AboutFragment()};
        FragmentTransaction fragmentTransaction = MainActivity.getInstance().getFragmentManager().beginTransaction();
        for (Fragment f : fragments) {
            fragmentTransaction.remove(f);
        }

        position = p;

        fragmentTransaction.replace(R.id.mainContainer, fragments[position - 1]);
        fragmentTransaction.commit();
    }

    public void showFragment() {
        if(position == 0) {
            return;
        }

        Fragment[] fragments = {new InterrupterFragment(), new MidiFragment(), new MoreFragment(), new AboutFragment()};
        FragmentTransaction fragmentTransaction = MainActivity.getInstance().getFragmentManager().beginTransaction();
        fragmentTransaction.replace(R.id.mainContainer, fragments[position - 1]);
        fragmentTransaction.commit();
    }
}
