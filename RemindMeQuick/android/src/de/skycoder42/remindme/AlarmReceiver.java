package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;

//DEBUG
import android.widget.Toast;

public class AlarmReceiver extends BroadcastReceiver {

	@Override
	public void onReceive(Context context, Intent intent) {
		Toast.makeText(context, "ALARM TRIGGERED!!!", Toast.LENGTH_LONG).show();
	}
}
