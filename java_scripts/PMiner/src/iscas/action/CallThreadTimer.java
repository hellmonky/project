package iscas.action;

import iscas.service.ConfigLoad;
import iscas.service.DataProcess;

import java.util.Timer;
import java.util.TimerTask;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class CallThreadTimer {
	
	private static final Logger logger = LoggerFactory.getLogger(CallThreadTimer.class);
	Timer localTimer;

	public CallThreadTimer(int seconds) {
		// Create local timer
		localTimer = new Timer();
		// Create local timer task and set the timer-parameter
		localTimer.schedule(new LocalTimerTask(), 0, seconds * 1000);
	}
	
	class LocalTimerTask extends TimerTask {
		@Override
		public void run() {
			DataProcess.CrawlData(ConfigLoad.getUrlList());
			// 获取合并后的数据文件
			DataProcess.GetCrawlData();
			logger.info("Crawl Data!");
		}
	}
}
