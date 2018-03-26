package de.skycoder42.remindme;

import android.content.Context;
import android.content.Intent;

import android.os.IBinder;
import android.os.Binder;
import android.os.Bundle;

import android.graphics.BitmapFactory;
import android.graphics.Color;

import android.support.v4.app.NotificationCompat;
import android.support.v4.app.RemoteInput;

import org.qtproject.qt5.android.bindings.QtService;

import android.util.Log;

public class RemindmeService extends QtService {
	private class ServiceCommand implements Runnable {
		@Override public void run() {
			RemindmeService.this.createQt();
		}
	}

	private Thread runThread;
	private final IBinder _binder = new Binder();

	@Override
	public void onCreateHook() {
		runThread = new Thread(new ServiceCommand());
		runThread.setDaemon(true);
		runThread.start();
		try {
			// TODO ugly, slows down (e.g. the complete part)
			// BUT was needed because otherwise native call fails...
			runThread.join(2500);
		} catch(Throwable e) {
			e.printStackTrace();
		}
	}

	public void createQt() {
		Log.d("RemindmeService", "createQt");
		super.onCreateHook();
		Log.d("RemindmeService", "super.onCreateHook() finally returned");
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		// explicitly exit to prevent the process from beeing cached
		System.exit(0);
	}

	@Override
	public IBinder onBind(Intent intent) {
		return _binder;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		int result = super.onStartCommand(intent, flags, startId);

		NotificationCompat.Builder builder = new NotificationCompat.Builder(this, Globals.ForegroundChannelId)
			.setContentTitle(getString(R.string.svc_name))
			.setContentText(getString(R.string.svc_desc))
			.setContentInfo(getString(R.string.app_name))
			.setLargeIcon(BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher))
			.setSmallIcon(R.drawable.ic_notification)
			.setColor(Globals.ForegroundColor)
			.setLocalOnly(true)
			.setOngoing(true)
			.setCategory(NotificationCompat.CATEGORY_REMINDER);

		if (!Globals.isOreo())
			builder.setPriority(NotificationCompat.PRIORITY_MIN);

		startForeground(Globals.ForegroundId, builder.build());
		if(intent != null)
			handleIntent(intent);

		return result;
	}

	public void completeAction() {
		stopForeground(true);
		stopService(new Intent(this, RemindmeService.class));//Stop myself
		try {
			runThread.join(2500);
		} catch(Throwable e) {
			e.printStackTrace();
		}
	}

	private static native void handleIntent(String action, String remId, int versionCode, String resultExtra);

	private void handleIntent(Intent intent) {
		String remId = intent.getStringExtra(Globals.ExtraId);
		int versionCode = intent.getIntExtra(Globals.ExtraVersion, 0);

		// cancel complete notifications early for a smoother experience
		// TODO maybe not needed? -> check again after join removed
		if(intent.getAction() == Globals.Actions.ActionComplete.getAction())
			Notifier.cancelExplicitly(this, remId);

		String resultExtra = null;
		if(intent.getAction() == Globals.Actions.ActionSnooze.getAction()) {
			Bundle remoteInput = RemoteInput.getResultsFromIntent(intent);
			if(remoteInput != null)
				resultExtra = remoteInput.getCharSequence(Globals.ExtraSnoozeTime).toString();
		}

		handleIntent(intent.getAction(), remId, versionCode, resultExtra);
	}
}
