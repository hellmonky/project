package iscas.action;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Iterator;
import java.util.List;

import iscas.entity.OfficeRecord;
import iscas.entity.Profile;
import iscas.entity.Tuple;
import iscas.service.ProfileService;

import javax.annotation.Resource;

import net.sf.json.JSONObject;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Controller;

import com.opensymphony.xwork2.ActionSupport;

@Controller
@Scope("prototype")
public class OfficeRankAction extends ActionSupport {
	
	private static final long serialVersionUID = 6391199588342151938L;
	@SuppressWarnings("unused")
	private static final Logger logger = LoggerFactory.getLogger(OfficeRankAction.class);
	
	@Resource
	private ProfileService profileService;
	
	private String id;
	private String name;
	private JSONObject rankInfo = new JSONObject();

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}
	
	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public JSONObject getRankInfo() {
		return rankInfo;
	}

	public void setRankInfo(JSONObject rankInfo) {
		this.rankInfo = rankInfo;
	}

	public String findRankList() {
		Profile pf = profileService.findProfile(id);
		rankInfoFromProfile(pf);
		
		return SUCCESS;
	}
	
	public String findRankListByName() {
		Profile pf = profileService.findProfiles(name).get(0);
		rankInfoFromProfile(pf);
		
		return SUCCESS;
	}
	
	public String findRankListById() {
		Profile pf = profileService.findProfile(id);
		rankInfoFromProfile(pf);
		
		return SUCCESS;
	}
	
	/*
	 * 从Profile实体对象中提取用于前台显示的rankInfo JSON格式数据。
	 */
	public void rankInfoFromProfile(Profile pf) {
		List<OfficeRecord> recordList = pf.getOfficeRecord();
		if (recordList == null) {
			rankInfo.put("startYear", "NODATA");
			return;
		}
		// 先过滤一遍recordList，日期不规范的全部去除，不加入统计
		Iterator<OfficeRecord> it = recordList.iterator();
		while(it.hasNext()) {
			OfficeRecord record = it.next();
			if (record.getStartDate().equals("---") || record.getEndDate().equals("---"))
				it.remove();
			else {
				String startYear = record.getStartDate().substring(0, 4);
				String endYear = record.getEndDate().substring(0, 4);
				int start = Integer.parseInt(startYear);
				int end = Integer.parseInt(endYear);
				if (!checkYear(start) || !checkYear(end))
					it.remove();
				if (start > end)
					it.remove();
			}
		}
		
		if (recordList.size() == 0) {
			rankInfo.put("startYear", "NODATA");
			return;
		}
		rankInfo.put("startYear", recordList.get(0).getStartDate().substring(0, 4));
		rankInfo.put("endYear", recordList.get(recordList.size() - 1).getEndDate().substring(0, 4));
		
		List<String> rankList = new ArrayList<String>();
		List<String> contentList = new ArrayList<String>();
		List<String> timeList = new ArrayList<String>();
		
		int nextStart = 0;
		nextStart = Integer.parseInt(recordList.get(0).getStartDate().substring(0, 4));
		
		for (int i = 0; i < recordList.size(); i++) {
			OfficeRecord record = recordList.get(i);
			
			String endYear = record.getEndDate().substring(0, 4);
			int start = nextStart;
			int end = Integer.parseInt(endYear);
			String rank = record.getRank();
			String timeInfo = record.getStartDate() + "~" + record.getEndDate();
			
			List<Tuple> tupleList = record.getTupleList();
			String content = "";
			if (tupleList != null && tupleList.size() > 0)
				for (Tuple tuple : tupleList)
					content += tuple.getContent() + "@";
			
			for (int j = start; j <= end; j++) {
				rankList.add(rank);
				contentList.add(content);
				timeList.add(timeInfo);
			}
			
			if (i + 1 < recordList.size()) {
				String nextStartYear = recordList.get(i + 1).getStartDate().substring(0, 4);
				nextStart = Integer.parseInt(nextStartYear);
			}
			
			while ((end + 1) < nextStart) {
				rankList.add(rank);
				contentList.add(content);
				timeList.add(timeInfo);
				end++;
			}
			nextStart = end + 1;
		}
		rankInfo.put("rankList", rankList);
		rankInfo.put("contentList", contentList);
		rankInfo.put("timeList", timeList);
		rankInfo.put("id", pf.getId());
		rankInfo.put("name", pf.getName());
		rankInfo.put("birthDate", pf.getBirthDate());
		return;
	}
	
	public boolean checkYear(int year) {
		Calendar c = Calendar.getInstance();
		int current = c.get(Calendar.YEAR); //得到当前年份
		
		if (year < 1900 || year > (current + 2))
			return false;
		else
			return true;
	}
}
