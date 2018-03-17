package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;
import android.content.BroadcastReceiver;
import android.os.Build;

public class BootReceiver extends BroadcastReceiver {

	@Override
	public void onReceive(Context context, Intent intent) {
		Intent startIntent = new Intent(RemindmeService.Actions.ActionScheduler.getAction(), null, context, RemindmeService.class);

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
			context.startForegroundService(startIntent);
		else
			context.startService(startIntent);
	}
}
