package iscas.service;

import java.io.InputStream;
import java.util.Properties;

public class ConfigLoad {
	static private String dataDir = null;
	static private String appointDir = null;
	static private String configTxt = null;
	static private String urlList = null;
	static private String deletedList = null;
	
	static {
		loads();
	}

	synchronized static public void loads() {
		if (urlList == null || configTxt == null) {
			InputStream is = ConfigLoad.class.getResourceAsStream("/config.properties");
			Properties dbProps = new Properties();
			try {
				dbProps.load(is);
				dataDir = dbProps.getProperty("dataDir");
				configTxt = dbProps.getProperty("configTxt");
				urlList = dbProps.getProperty("urlList");
				appointDir = dbProps.getProperty("appointDir");
				deletedList = dbProps.getProperty("deletedList");
			}
			catch (Exception e) {
				System.err.println("不能读取属性文件. " +	"请确保db.properties在CLASSPATH指定的路径中");
			}
		}
	}

	public static String getDataDir() {
		if (dataDir == null)
			loads();
		return dataDir;
	}
	
	public static String getAppointDir() {
		if (appointDir == null)
			loads();
		return appointDir;
	}
	
	public static String getConfigTxt() {
		if (configTxt == null)
			loads();
		return configTxt;
	}
	
	public static String getUrlList() {
		if (urlList == null)
			loads();
		return urlList;
	}
	
	public static String getDeletedList() {
		if (deletedList == null)
			loads();
		return deletedList;
	}
}