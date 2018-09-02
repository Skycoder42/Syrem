package de.skycoder42.syrem;

import android.content.Intent;

import android.os.Bundle;

import android.graphics.BitmapFactory;

import android.support.v4.app.NotificationCompat;
import android.support.v4.app.RemoteInput;

import de.skycoder42.qtservice.AndroidService;

public class SyremService extends AndroidService {
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		int result = super.onStartCommand(intent, flags, startId);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(this, Globals.ForegroundChannelId)
			.setContentTitle(getString(R.string.svc_name))
			.setContentText(getString(R.string.svc_desc))
			.setContentInfo(getString(R.string.app_name))
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.mipmap.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setColor(Globals.ForegroundColor)
			.setLocalOnly(true)
			.setOngoing(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER);

		if (!Globals.isOreo())
			builder.setPriority(NotificationCompat.PRIORITY_MIN);

		startForeground(Globals.ForegroundId, builder.build());
		return result;
	}

	public void completeAction(int startId) {
		if(stopSelfResult(startId))
			stopForeground(true);
	}

	public String handleIntent(Intent intent) {
		// cancel complete notifications early for a smoother experience
		if(intent.getAction() == Globals.Actions.ActionComplete.getAction()) {
			String remId = intent.getStringExtra(Globals.ExtraId);
			Notifier.cancelExplicitly(this, remId);
		}

		// extract the snooze string from the intent if set
		String resultExtra = null;
		if(intent.getAction() == Globals.Actions.ActionSnooze.getAction()) {
			Bundle remoteInput = RemoteInput.getResultsFromIntent(intent);
			if(remoteInput != null)
				resultExtra = remoteInput.getCharSequence(Globals.ExtraSnoozeTime).toString();
		}
		return resultExtra;
	}
}
