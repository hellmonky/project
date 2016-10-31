package iscas.service;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class TimeDiffService {
	
	/**
	 * 这里共有2个时间段（b1-----e1）【b2-----e2】，4个时间点；
	 *相当于两条线段(b代表起点，e代表端点，b<=e)，4个端点。
	 *
	 *可分3种情况：
	 *1.包含(两条)
	 *（b1---【b2-----e2】--e1）   if(b1<=b2 && e1>=e2)
	 *【b2---（b1-----e1）--e2】   if(b1>=b2&&e1<=e2)
	 *
	 *2.相交(两条)
	 *（b1---【b2---e1）----e2】    if(b1<b2&&e1<e2&&e1>b2)
	 *【b2---(b1---e2】----e1)	 if(b1>b2&&b1<e2&&e2<e1)
	 *
	 *3.不相交
	 *（b1-----e1）【b2-----e2】或【b2-----e2】（b1-----e1）if(e1<b2||b1>e2)此时，重合时间数为零。
	 * @param beginTime1
	 * @param endTime1
	 * @param beginTime2
	 * @param endTime2
	 * @return
	 */
	public static String getTimeCoincidence(Date beginTime1,Date endTime1,Date beginTime2,Date endTime2){
		long b1 = beginTime1.getTime();
		long e1 = endTime1.getTime();
		long b2 = beginTime2.getTime();
		long e2 = endTime2.getTime();
		
		String coincidenceTime;
		
		if(b1<=b2&&e1>=e2){//（b1---【b2-----e2】--e1）1包含2
			coincidenceTime=getTimeDifference(endTime2,beginTime2);
		}else if(b1>=b2&&e1<=e2){//【b2---（b1-----e1）--e2】2包含1
			coincidenceTime=getTimeDifference(endTime1,beginTime1);
		}else if(b1>=b2&&b1<=e2&&e2<=e1){//（b1---【b2---e1）----e2】 相交1
			coincidenceTime=getTimeDifference(endTime2,beginTime1);
		}else if(b1<=b2&&e1<=e2&&e1>=b2){//【b2---(b1---e2】----e1) 相交2
			coincidenceTime=getTimeDifference(endTime1,beginTime2);
		}else if(e1<=b2||b1>=e2){//（b1-----e1）【b2-----e2】或【b2-----e2】（b1-----e1）不相交
			coincidenceTime="0";
		}else{
			coincidenceTime="";
			System.out.println("意料外的日期组合，无法计算重合！");
		}
		if (coincidenceTime.equals(""))
			coincidenceTime = "0";
		return coincidenceTime;
	}
	
	public static String timeFormatCheck(String time) {
		String result = time;
		Calendar c = Calendar.getInstance();
		int year = c.get(Calendar.YEAR); 
		int month = c.get(Calendar.MONTH);
		
		if (time == null || time.equals("---") || time.equals("")) {
			result = String.valueOf(year) + "-" + String.valueOf(month);
		} else {
			int index = time.indexOf("--");
			if (index >= 0) {
				result = time.replace("--", "-" + String.valueOf(month));
			}
		}
		return result;
	}
	
	public static String getTimeCoincidence(String beginTime1, String endTime1, String beginTime2, String endTime2){
		beginTime1 = timeFormatCheck(beginTime1);
		beginTime2 = timeFormatCheck(beginTime2);
		endTime1 = timeFormatCheck(endTime1);
		endTime2 = timeFormatCheck(endTime2);
		
		Date beginDate1 = stringToDate(beginTime1);
		Date endDate1 = stringToDate(endTime1);
		Date beginDate2 = stringToDate(beginTime2);
		Date endDate2 = stringToDate(endTime2);
		
		return getTimeCoincidence(beginDate1, endDate1, beginDate2, endDate2);
	}
	
	/**
	 * 计算两个时间差
	 * @param date1
	 * @param date2
	 * @return
	 */
	@SuppressWarnings("deprecation")
	public static String getTimeDifference(Date date1,Date date2){
		StringBuffer df = new StringBuffer();	
		try{
			//long num = (date1.getTime()-date2.getTime())/(1000*60);
			long num = (date1.getYear() - date2.getYear()) * 12 + date1.getMonth() - date2.getMonth();

			if (num >= 0)
				df.append(num);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return df.toString();
	}
	
	/**
	 * 字符串转时间格式
	 * @param date
	 * @return
	 */
	public static Date stringToDate(String date) {
		if (date == null)
			return null;

		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM");
		Date strToDate = null;
		try {
			strToDate = format.parse(date);
		} catch (ParseException e) {
			e.printStackTrace();
		}
		return strToDate;
	}
}
