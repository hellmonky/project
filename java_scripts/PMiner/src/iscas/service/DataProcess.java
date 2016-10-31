package iscas.service;

import service.Crawler;
import iscas.entity.News;

import java.io.File;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class DataProcess {
	@SuppressWarnings("unused")
	private static final Logger logger = LoggerFactory.getLogger(DataProcess.class);
	
	private static List<News> newsList=new ArrayList<News>();
	private static List<News> appointList=new ArrayList<News>();
	
	public static List<News> getNewsList() {
		return newsList;
	}
	
	public static void setNewsList(List<News> newsList) {
		DataProcess.newsList = newsList;
	}
	
	public static List<News> getAppointList() {
		return appointList;
	}
	
	public static void setAppointList(List<News> appointList) {
		DataProcess.appointList = appointList;
	}
	
	// 获取目标URL中的信息
	/*
	 * 参数说明：
	 * strURL：目标网址
	 * iType：获取信息类型，0-任免信息，1-活动新闻类信息
	 */
	public static void CrawURL(String strURL, int iType){
		String strTemp = "", strPath = "", strCondi = "", strTime = null, strWordNum = "";
		// 获取抓取内容类型条件
		strCondi = GetConfigContent(ConfigLoad.getConfigTxt(), "condition");
		if (iType == 0)	{
			strCondi = "任 免";
		}
		// 读取文件输出路径
		strPath = GetConfigContent(ConfigLoad.getConfigTxt(), "path");
		// 获取当前时间
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd");	// 设置日期格式
		Calendar c = Calendar.getInstance();						// 获取当前系统时间
		strPath += "/" + df.format(c.getTime());
		File fi = new File(strPath);
		if (! fi.exists()) {
			if (!fi.mkdir()) {
    			System.out.println("创建文件夹失败！");
    			return;
    		}
		}
		
		// 获取URL中各地名称
		int iIndex = strURL.indexOf(".gov");
		strTemp = strURL.substring(0, iIndex);
		iIndex = strTemp.indexOf(".");
		strTemp = strTemp.substring(iIndex + 1);
		String strPathTemp = "";
		if (iType == 0) {
			strPath += "/" + strTemp + "-rm" + ".txt";
		} else {
			strPathTemp = strPath + "/" + strTemp + "-news" + ".txt";
			// 判断本地是否已存在该文件
			File fTemp = new File(strPathTemp);
			if (fTemp.exists()) {
				iIndex = strURL.indexOf("index_");
				if (iIndex > -1) {
					strTemp += strURL.substring(iIndex + 6, iIndex + 8);
				}
				strPath += "/" + strTemp + "-news" + ".txt";
			} else {
				strPath = strPathTemp;
			}
		}		
		
		// 读取内容字数
		strWordNum = GetConfigContent(ConfigLoad.getConfigTxt(), "wordNum");
		int iNum = Integer.parseInt(strWordNum);
		// 读取时间
		strTime = GetConfigContent(ConfigLoad.getConfigTxt(), "date");
		SimpleDateFormat dfd = new SimpleDateFormat("yyyy-MM-dd HH:mm");	// 设置日期格式
		Calendar ct = Calendar.getInstance();								// 获取当前系统时间
		int iDate = Integer.parseInt(strTime);
		ct.add(Calendar.DATE, -iDate);
		strTime = dfd.format(ct.getTime());
		
		// 调用爬虫接口
		Crawler.getInfo(strURL, strCondi, strPath, iNum, strTime);
	}
	
	
	// 抓取配置文件中所有url中的数据
	public static void CrawlData(String strURLConfigFile){
		String strTemp = "";
		int iType = 0;
		try {
			// 获取URL列表中的网址
			FileReader fr = new FileReader(strURLConfigFile);
			// 读取文件
			BufferedReader br = new BufferedReader(fr);
			strTemp = br.readLine();
			while (strTemp != null) {
				// 任免、新闻活动标识行
				if (strTemp.equals("任免") || strTemp.equals("新闻活动")) {
					if (strTemp.equals("新闻活动")) {
						iType = 1;
					}
					strTemp = br.readLine();
				}
				// 抓取对应URL中的内容并输出
				CrawURL(strTemp, iType);
				strTemp = br.readLine();
			}
			br.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	
	// 过滤、剔除无用信息并分类合并爬取到的各URL中的数据
	public static void GetCrawlData() {
		newsList.clear();
		appointList.clear();
		// 获取数据目录下的数据文件，分类合并输出
		String strPath = GetConfigContent(ConfigLoad.getConfigTxt(), "path");
		// 获取当前时间
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd"); // 设置日期格式
		Calendar c = Calendar.getInstance(); // 获取当前系统时间
		strPath += "/" + df.format(c.getTime());
		File fl = new File(strPath);
		File[] list = fl.listFiles();
		// 创建输出文件路径
		File fi = new File(strPath);
		if (!fi.exists()) {
			if (!fi.mkdir()) {
				System.out.println("创建文件夹失败！");
				return;
			}
		}
		// 创建输出文件，如果本地已经存在，先删除再创建
		String strRMFilePath = strPath + "/" + "rmxx.txt";
		String strnewsPath = strPath + "/" + "news.txt";
		File fRM = new File(strRMFilePath);
		if (fRM.exists()) {
			fRM.delete();
		}
		File fNews = new File(strnewsPath);
		if (fNews.exists()) {
			fNews.delete();
		}

		for (int i = 0; i < list.length; i++) {
			if (list[i].isFile()) {
				strPath = list[i].getPath();
				String strTemp = "";
				if (list[i].getName().equals("news.txt")
						|| list[i].getName().equals("rmxx.txt")) {
					continue;
				} else if (strPath.contains("-news")) {
					strTemp = strnewsPath;
				} else {
					strTemp = strRMFilePath;
				}
				// 调用分析接口提取、过滤数据，并分类合并输出
				DataAnalyse(strPath, strTemp);
			}
		}
	}
	
	
	public static List<News> getNewsData(){
		// 获取数据目录下的数据文件，分类合并输出
		String strPath = GetConfigContent(ConfigLoad.getConfigTxt(), "path");
		// 获取当前时间
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd"); // 设置日期格式
		Calendar c = Calendar.getInstance(); // 获取当前系统时间
		strPath += "/" + df.format(c.getTime());
		File fl = new File(strPath);
		File[] list = fl.listFiles();

		// 创建输出文件路径
		File fi = new File(strPath);
		if (!fi.exists()) {
			if (!fi.mkdir()) {
				System.out.println("创建文件夹失败！");
				return null;
			}
		}
		// 创建输出文件，如果本地已经存在，先删除再创建
		String strnewsPath = strPath + "/" + "news.txt";
		File fNews = new File(strnewsPath);
		if (fNews.exists()) {
			fNews.delete();
		}

		for (int i = 0; i < list.length; i++) {
			if (list[i].isFile()) {
				strPath = list[i].getPath();
				if (list[i].getName().equals("rmxx.txt")
						|| list[i].getName().equals("-rm.txt")
						|| list[i].getName().equals("news.txt")) {
					continue;
				}

				// 调用分析接口提取、过滤数据，并分类合并输出
				DataAnalyse(strPath, strnewsPath);
			}
		}
		return newsList;
	}
	
	
	public static List<News> getAppointData(){
		// 获取数据目录下的数据文件，分类合并输出
		String strPath = GetConfigContent(ConfigLoad.getConfigTxt(), "path");
		// 获取当前时间
		SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd"); // 设置日期格式
		Calendar c = Calendar.getInstance(); // 获取当前系统时间
		strPath += "/" + df.format(c.getTime());
		File fl = new File(strPath);
		File[] list = fl.listFiles();

		// 创建输出文件路径
		File fi = new File(strPath);
		if (!fi.exists()) {
			if (!fi.mkdir()) {
				System.out.println("创建文件夹失败！");
				return null;
			}
		}
		// 创建输出文件，如果本地已经存在，先删除再创建
		String strRMPath = strPath + "/" + "rmxx.txt";
		File fNews = new File(strRMPath);
		if (fNews.exists()) {
			fNews.delete();
		}

		for (int i = 0; i < list.length; i++) {
			if (list[i].isFile()) {
				strPath = list[i].getPath();
				if (list[i].getName().equals("rmxx.txt")
						|| list[i].getName().equals("-news.txt")
						|| list[i].getName().equals("news.txt")) {
					continue;
				}

				// 调用分析接口提取、过滤数据，并分类合并输出
				DataAnalyse(strPath, strRMPath);
			}
		}
		return appointList;
	}
	
	
	// 提取、过滤给定文件中的信息
	/*
	 * 输入参数：
	 * 		strFilePath：原文件路径
	 * 		strOutFilePath：合并后的文件路径
	 */
	public static void DataAnalyse(String strFilePath, String strOutFilePath) {
		int iIndex = 0;
		String strTemp = "";
		// 根据输入的文件名提取各省份的名称，如js-江苏省,hunan-湖南省,gd-广东省
		String strProvinceName = "";
		if (strFilePath.contains("js")) {
			strProvinceName = "江苏省";
		}
		if (strFilePath.contains("hunan")) {
			strProvinceName = "湖南省";
		}
		if (strFilePath.contains("gd")) {
			strProvinceName = "广东省";
		}

		try {
			File f = new File(strFilePath);
			InputStreamReader read = new InputStreamReader(new FileInputStream(
					f), "UTF-8");
			// 读取文件
			BufferedReader br = new BufferedReader(read);
			strTemp = br.readLine();
			while (strTemp != null) {
				if (strTemp.equals("")) {
					strTemp = br.readLine();
					continue;
				}
				// 分析数据段中各行数据内容，提取有用信息
				String strTitle = "", strURL = "", strDate = "", strContent = "";
				// 解析内容
				strTitle = strTemp; // 标题行
				strURL = br.readLine(); // url
				strDate = br.readLine(); // 日期
				strContent = br.readLine(); // 内容

				// 剔除无用数据
				if (!strDate.equals("") || !strContent.equals("")) {
					// 判断是否提取到时间，如果没有提取到，则需要从URL中提取时间
					iIndex = strURL.indexOf("/t20"); // 如果URL中不含有时间字段，则判断为无用信息，过滤之
					if (iIndex < 0) {
						strTemp = br.readLine();
						continue;
					}
					if (!strDate.contains("|||")) // 如果时间字段信息不全，则补全时间，从URL中获取
					{
						strTemp = strURL.substring(iIndex + 2);
						iIndex = strTemp.indexOf("_");
						strTemp = strTemp.substring(0, iIndex);
						String strYear = strTemp.substring(0, 4);
						String strMonth = strTemp.substring(4, 6);
						String strDay = strTemp.substring(6);
						strDate += strYear + "年" + strMonth + "月" + strDay + "日" + "|||";
					}

					// 打开可追加写入的输出文件
					File fout = new File(strOutFilePath);
					if (!fout.exists()) {
						fout.createNewFile();
					}
					OutputStreamWriter write = new OutputStreamWriter(
							new FileOutputStream(fout, true), "UTF-8");
					BufferedWriter writer = new BufferedWriter(write);

					iIndex = strContent.indexOf("决定：");
					// 如果标题信息中含有相关关键字则从内容字段中提取任免信息
					if ((strTitle.contains("等同志") || strTitle.contains("任前公示")
							|| strTitle.contains("任免名单") || strTitle
								.contains("通知"))
							&& strFilePath.contains("-rm")
							&& iIndex > -1) {
						strTemp = strContent.substring(iIndex + 3);
						iIndex = strTemp.indexOf("；");
						while (iIndex > -1) {
							// 提取对应的任免信息
							strTitle = strTemp.substring(0, iIndex);
							strTitle.trim();
							strTitle = strProvinceName + strTitle;
							// 数据写入文件
							writer.write(strTitle + "\r\n"); // 标题
							writer.write(strURL + "\r\n"); // URL
							writer.write(strDate + "\r\n"); // 时间
							writer.write(strContent + "\r\n\r\n\r\n");
							// 将各数据加入list中
							if (strFilePath.contains("-rm")) {
								News news = new News();
								news.setTitle(strTitle.substring(0,
										strTitle.length() - 3));
								news.setContentHref(strURL.substring(0,
										strURL.length() - 3));
								news.setDate(strDate.substring(0,
										strDate.length() - 3));
								appointList.add(news);
							}
							strTemp = strTemp.substring(iIndex + 1);
							iIndex = strTemp.indexOf("；");
						}
					} else {
						// 目前对其他较标准的任免标题或新闻类信息暂不做处理，直接输出数据写入文件
						writer.write(strTitle + "\r\n"); // 标题
						writer.write(strURL + "\r\n"); // URL
						writer.write(strDate + "\r\n"); // 时间
						writer.write(strContent + "\r\n\r\n\r\n"); // 内容
						// 将各数据加入list中
						News news = new News();
						news.setTitle(strTitle.substring(0,
								strTitle.length() - 3));
						news.setContentHref(strURL.substring(0,
								strURL.length() - 3));
						news.setDate(strDate.substring(0, strDate.length() - 3));
						if (strFilePath.contains("-rm")) {
							appointList.add(news);
						}
						if (strFilePath.contains("-news")) {
							newsList.add(news);
						}
					}
					writer.close();
				}
				strTemp = br.readLine();
			}
			br.close();

		} catch (IOException e) {
			System.out.println(e.toString());
		}
	}
	
	
	// 读取配置文件中对应的字段内容
	/*
	 * 输入参数：strConfigFilePath,配置文件路径
	 * 		    strKeyWork，配置文件中对应的字段名称，如：condition、path、wordNum、date
	 * 输出：返回值为对应字段的值
	 */
	public static String GetConfigContent(String strConfigFilePath,	String strKeyWork) {
		String strContent = "";
		try {
			FileReader fr = new FileReader(strConfigFilePath);
			// 读取文件
			BufferedReader br = new BufferedReader(fr);
			strContent = br.readLine();
			while (strContent != null) {
				if (strContent.contains(strKeyWork)) {
					int iIndex = strContent.indexOf("=");
					strContent = strContent.substring(iIndex + 1);
					break;
				}
				strContent = br.readLine();
			}
			br.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return strContent;
	}
}
