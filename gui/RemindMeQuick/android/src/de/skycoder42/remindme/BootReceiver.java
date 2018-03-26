package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;

public class BootReceiver extends BroadcastReceiver {
	@Override
	public void onReceive(Context context, Intent intent) {
		Intent startIntent = new Intent(Globals.Actions.ActionSetup.getAction(), null, context, RemindmeService.class);
		if(Globals.isOreo())
			context.startForegroundService(startIntent);
		else
			context.startService(startIntent);
	}
}
