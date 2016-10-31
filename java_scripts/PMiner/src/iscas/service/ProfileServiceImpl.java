package iscas.service;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import iscas.dao.ProfileBackupDao;
import iscas.dao.ProfileDao;
import iscas.entity.OfficeRecord;
import iscas.entity.Profile;
import iscas.entity.Relation;
import iscas.entity.StudyRecord;
import iscas.entity.Tuple;

public class ProfileServiceImpl implements ProfileService {
	
	private ProfileDao profileDao;
	private ProfileBackupDao profileBackupDao;

	public ProfileDao getProfileDao() {
		return profileDao;
	}

	public void setProfileDao(ProfileDao profileDao) {
		this.profileDao = profileDao;
	}

	public ProfileBackupDao getProfileBackupDao() {
		return profileBackupDao;
	}

	public void setProfileBackupDao(ProfileBackupDao profileBackupDao) {
		this.profileBackupDao = profileBackupDao;
	}

	@Override
	public List<Profile> findProfiles(String name) {
		return profileDao.findByName(name);
	}

	@Override
	public Profile findProfile(String id) {
		return profileDao.findById(id);
	}

	@Override
	public void saveProfile(Profile profile) {
		profileDao.saveOrModify(profile);
		profileBackupDao.saveOrModify(profile);
	}
	
	@Override
	public boolean deleteProfile(String id) {
		if (profileDao.deleteProfile(id))
			return true;
		else
			return false;
	}

	@Override
	// 根据人员id查找其同乡关系信息
	public JSONArray findLocMate(String id) {
		JSONArray jsonLocationRelation = new JSONArray();
		
		Profile self = findProfile(id);
		String selfName = self.getName();
		// 先判断self的信息的完整性和合法性
		if (self.getBirthProvince().equals("-") 
				&& self.getBirthCity().equals("-")
				&& self.getBirthDistrict().equals("-")) {
			return jsonLocationRelation;
		}
		
		JSONArray locMateInfo = profileDao.findLocMate(self.getBirthProvince());
		// 对JSONArray进行处理，得到前台表示层需要的信息格式
		// relation： {source, target, weight}
		for (int i = 0; i < locMateInfo.size(); i++) {
			JSONObject object = locMateInfo.getJSONObject(i);
			
			// 排除自己的id
			if (!object.getString("_id").equals(id)) {
				JSONObject item = new JSONObject();
				item.put("source", object.getString("name"));
				item.put("target", selfName);
				
				// 根据省市区的相同程度确定weight值，分别是9,6,3
				// 其中9代表关系最近，即省市区都符合，其他类似
				if (object.getString("birthCity").equals(self.getBirthCity())) {
					if (object.getString("birthDistrict").equals(self.getBirthDistrict()))
						item.put("weight", "9");
					else
						item.put("weight", "6");
				} else {
					item.put("weight", "3");
				}
				
				jsonLocationRelation.add(item);
			}
		}
		
		return jsonLocationRelation;
	}

	@Override
	// 根据人员id查找其同事关系信息，以工作时间作为权重
	public JSONArray findColleague(String id) {
		
		Map<String, Relation> relationMap = new HashMap<String, Relation>();
		List<Relation> relationList = new ArrayList<Relation>();

		Profile self = findProfile(id);
		String selfName = self.getName();
		// 根据工作单位的名称找寻同事关系
		// 根据工作时间计算关系远近 - weight
		List<OfficeRecord> list = self.getOfficeRecord();
		if (list == null)
			return JSONArray.fromObject(relationList);
		
		for (OfficeRecord record : list) {
			
			String selfStartDate = record.getStartDate();
			String selfEndDate = record.getEndDate();
			
			List<Tuple> tupleList = record.getTupleList();
			if (selfStartDate == null || selfEndDate == null || tupleList == null)
				continue;
			
			for (Tuple tuple : tupleList) {
				if (tuple.getUnitNameList() == null || tuple.getUnitNameList().size() == 0)
					continue;
				String selfUnitName = tuple.getUnitNameList().get(0); // 取第一级的单位名称
				// 先判断self的单位名称的合法性
				if (selfUnitName == null || selfUnitName.equals("-"))
					continue;
				// 根据单位名称查找在同一单位工作过的人员信息
				JSONArray colleagueInfo = profileDao.findColleague(selfUnitName);
				
				for (int i = 0; i < colleagueInfo.size(); i++) {
					JSONObject object = colleagueInfo.getJSONObject(i);
					// 排除自己的id
					if (!object.getString("_id").equals(id)) {
						String colleagueName = object.getString("name");
						// 计算weight
						JSONArray recordArray = object.getJSONArray("officeRecord");
						// officeRecord可能有多项，都是在同一单位工作
						// 采取相加的策略计算权重值weight
						for (int j = 0; j < recordArray.size(); j++) {
							JSONObject recordObject = recordArray.getJSONObject(j);
							// 获取这一段工作经历的开始时间和结束时间
							String startDate = recordObject.getString("startDate");
							String endDate = recordObject.getString("endDate");
							// 计算时间区段差值作为权重
							String timeDiff = TimeDiffService.getTimeCoincidence(selfStartDate, selfEndDate, startDate, endDate);
							
							Relation relation = new Relation();
							relation.setSource(colleagueName);
							relation.setTarget(selfName);
							relation.setWeight(timeDiff);
							
							//如果存在同一人在两个时间段与self是同事的，weight值相加
							if (relationMap.get(colleagueName) == null) {
								relationMap.put(colleagueName, relation);
							} else {
								String originalWeight = relationMap.get(colleagueName).getWeight();
								relation.setWeight(String.valueOf(Integer.parseInt(timeDiff) + Integer.parseInt(originalWeight))); 
								relationMap.put(colleagueName, relation);
							}
							
						}
					}
				}
			}
		}
		for (Relation r : relationMap.values()) {
			relationList.add(r);
		}
		return JSONArray.fromObject(relationList);
	}

	@Override
	// 根据人员id查找其同事关系信息，以级别差值作为权重
	public JSONArray findRankColleague(String id) {
		Profile self = findProfile(id);
		String selfName = self.getName();
		
		Map<String, Relation> relationMap = new HashMap<String, Relation>();
		List<Relation> relationList = new ArrayList<Relation>();
		// 根据工作单位的名称找寻同事关系
		// 根据级别计算关系远近 - weight
		List<OfficeRecord> list = self.getOfficeRecord();
		if (list == null)
			return JSONArray.fromObject(relationList);
		for (OfficeRecord record : list) {
			List<Tuple> tupleList = record.getTupleList();
			
			String selfRank = record.getRank();
			
			for (Tuple tuple : tupleList) {
				if (tuple.getUnitNameList() == null || tuple.getUnitNameList().size() == 0)
					continue;
				String selfUnitName = tuple.getUnitNameList().get(0); // 取第一级的单位名称
				// 先判断self的单位名称的合法性
				if (selfUnitName == null || selfUnitName.equals("-"))
					continue;
				// 根据单位名称查找在同一单位工作过的人员信息
				JSONArray colleagueInfo = profileDao.findColleague(selfUnitName);
				
				for (int i = 0; i < colleagueInfo.size(); i++) {
					JSONObject object = colleagueInfo.getJSONObject(i);
					// 排除自己的id
					if (!object.getString("_id").equals(id)) {
						String colleagueName = object.getString("name");
											
						// 计算weight
						JSONArray recordArray = object.getJSONArray("officeRecord");
						// officeRecord可能有多项，都是在同一单位工作
						// 采取相加的策略计算权重值weight
						for (int j = 0; j < recordArray.size(); j++) {
							JSONObject recordObject = recordArray.getJSONObject(j);

							String colleagueRank = recordObject.getString("rank");
							String rankDiff = String.valueOf(11 - Math.abs(Integer.parseInt(selfRank) - Integer.parseInt(colleagueRank)));
							
							Relation relation = new Relation();
							relation.setSource(colleagueName);
							relation.setTarget(selfName);
							relation.setWeight(rankDiff);
							
							//如果存在同一人在两个时间段与self是同事的，取几个rankDiff的最大值确定最终weight
							if (relationMap.get(colleagueName) == null) {
								//if (!relation.getWeight().equals("0"))
								relationMap.put(colleagueName, relation);
							} else {
								String originalWeight = relationMap.get(colleagueName).getWeight();
								relation.setWeight(String.valueOf(Math.max(Integer.parseInt(rankDiff), Integer.parseInt(originalWeight)))); 
								relationMap.put(colleagueName, relation);
							}
						}				
						
					}
				}
			}
		}
		for (Relation r : relationMap.values()) {
			relationList.add(r);
		}
		return JSONArray.fromObject(relationList);
	}

	@Override
	// 根据人员id查找其过去的同学关系信息
	public JSONArray findPreviousSchoolMate(String id) {
		Map<String, Relation> relationMap = new HashMap<String, Relation>();
		List<Relation> relationList = new ArrayList<Relation>();

		Profile self = findProfile(id);
		String selfName = self.getName();
		// 根据学校的名称找寻同事关系
		List<StudyRecord> list = self.getStudyRecord();
		if (list == null)
			return JSONArray.fromObject(relationList);
		for (StudyRecord record : list) {
			if (record.getStartDate() != null && record.getEndDate() != null) {
				// 开始处理过去的学习经历
				String universityName = record.getUniversityName();		
				// 先判断self的单位名称的合法性
				if (universityName == null || universityName.equals("-"))
					continue;
				
				String selfStartDate = record.getStartDate();
				String selfEndDate = record.getEndDate();
				
				JSONArray schoolMateInfo = profileDao.findSchoolMate(universityName);
				for (int i = 0; i < schoolMateInfo.size(); i++) {
					JSONObject object = schoolMateInfo.getJSONObject(i);
					// 排除自己的id
					if (!object.getString("_id").equals(id)) {
						String schoolMateName = object.getString("name");
						
						// 计算weight
						JSONArray recordArray = object.getJSONArray("studyRecord");
						// studyRecord可能有多项，都是在同一大学学习
						// 同样采取相加的策略计算权重值weight
						for (int j = 0; j < recordArray.size(); j++) {
							JSONObject recordObject = recordArray.getJSONObject(j);
							
							String timeDiff = new String("10"); // 默认权重值
							// 获取这一段学习经历的开始时间和结束时间
							if (recordObject.containsKey("startDate") && recordObject.containsKey("endDate")) {
								String startDate = recordObject.getString("startDate");
								String endDate = recordObject.getString("endDate");
								// 计算时间区段差值作为权重
								timeDiff = TimeDiffService.getTimeCoincidence(selfStartDate, selfEndDate, startDate, endDate);
							}
							
							Relation relation = new Relation();
							relation.setSource(schoolMateName);
							relation.setTarget(selfName);
							relation.setWeight(timeDiff);
							
							//如果存在同一人在两个时间段与self是同学的，weight值相加
							if (relationMap.get(schoolMateName) == null) {
								relationMap.put(schoolMateName, relation);
							} else {
								String originalWeight = relationMap.get(schoolMateName).getWeight();
								relation.setWeight(String.valueOf(Integer.parseInt(timeDiff) + Integer.parseInt(originalWeight))); 
								relationMap.put(schoolMateName, relation);
							}
							
						}
						
					}
				}
			}
		}
		for (Relation r : relationMap.values()) {
			relationList.add(r);
		}
		return JSONArray.fromObject(relationList);
	}

	@Override
	// 根据人员id查找其现在的同事关系信息
	public JSONArray findCurrentSchoolMate(String id) {
		JSONArray jsonSchoolMateRelation = new JSONArray();

		Profile self = findProfile(id);
		String selfName = self.getName();
		// 根据学校的名称找寻同学关系
		// 一般取studyRecord的第一项为当前学习经历，或者判断没有起始时间和结束时间的经历
		List<StudyRecord> list = self.getStudyRecord();
		if (list == null)
			return jsonSchoolMateRelation;
		for (StudyRecord record : list) {
			if (record.getStartDate() == null && record.getEndDate() == null) {
				// 开始处理当前学习经历
				String universityName = record.getUniversityName();
				
				// 先判断self的学校名称的合法性
				if (universityName == null || universityName.equals("-"))
					continue;
				
				JSONArray schoolMateInfo = profileDao.findSchoolMate(universityName);
				for (int i = 0; i < schoolMateInfo.size(); i++) {
					JSONObject object = schoolMateInfo.getJSONObject(i);
					// 排除自己的id
					if (!object.getString("_id").equals(id)) {
						JSONObject item = new JSONObject();
						item.put("source", object.getString("name"));
						item.put("target", selfName);
						item.put("weight", "10");
						jsonSchoolMateRelation.add(item);
					}
				}
			}
		}
		return jsonSchoolMateRelation;
	}
	
	@Override
	public JSONArray findAllSchoolMate(String id) {
		//对综合关系中的同学关系需要重新计算处理，查找过去和现在的所有同学关系
		Map<String, Relation> relationMap = new HashMap<String, Relation>();
		List<Relation> relationList = new ArrayList<Relation>();
		
		Profile self = findProfile(id);
		String selfName = self.getName();
		
		List<StudyRecord> list = self.getStudyRecord();
		if (list == null)
			return JSONArray.fromObject(relationList);
		for (StudyRecord record : list) {
			String universityName = record.getUniversityName();
			// 先判断self的学校名称的合法性
			if (universityName == null || universityName.equals("-"))
				continue;
			
			JSONArray schoolMateInfo = profileDao.findSchoolMate(universityName);
			
			String selfStartDate = record.getStartDate();
			String selfEndDate = record.getEndDate();
			
			for (int i = 0; i < schoolMateInfo.size(); i++) {
				JSONObject object = schoolMateInfo.getJSONObject(i);
				// 排除自己的id
				if (!object.getString("_id").equals(id)) {
					String schoolMateName = object.getString("name");
					
					// 计算weight
					JSONArray recordArray = object.getJSONArray("studyRecord");
					// studyRecord可能有多项，都是在同一大学学习
					// 同样采取相加的策略计算权重值weight
					for (int j = 0; j < recordArray.size(); j++) {
						JSONObject recordObject = recordArray.getJSONObject(j);
						
						String timeDiff = new String("10"); // 默认权重值
						// 获取这一段学习经历的开始时间和结束时间
						if (recordObject.containsKey("startDate") && recordObject.containsKey("endDate") && selfStartDate != null && selfEndDate != null) {
							String startDate = recordObject.getString("startDate");
							String endDate = recordObject.getString("endDate");
							// 计算时间区段差值作为权重
							timeDiff = TimeDiffService.getTimeCoincidence(selfStartDate, selfEndDate, startDate, endDate);
						}
						
						Relation relation = new Relation();
						relation.setSource(schoolMateName);
						relation.setTarget(selfName);
						relation.setWeight(timeDiff);
						
						//如果存在同一人在两个时间段与self是同学的，weight值相加
						if (relationMap.get(schoolMateName) == null) {
							relationMap.put(schoolMateName, relation);
						} else {
							String originalWeight = relationMap.get(schoolMateName).getWeight();
							relation.setWeight(String.valueOf(Integer.parseInt(timeDiff) + Integer.parseInt(originalWeight))); 
							relationMap.put(schoolMateName, relation);
						}
					}
				}
			}
		}
		for (Relation r : relationMap.values()) {
			relationList.add(r);
		}
		return JSONArray.fromObject(relationList);
	}
	
	@Override
	public JSONObject findAllRelation(String id) {
		// 需要依赖前面几个函数的查询结果
		JSONArray jsonLocMateRelation = findLocMate(id);
		JSONArray jsonColleagueRelation = findColleague(id);
		JSONArray jsonSchoolMateRelation = findAllSchoolMate(id);
		
		//   name -> [同乡，同事，同学weight]
		Map<String, ArrayList<String>> mapAll = new HashMap<String, ArrayList<String>>();
		
		for (int i = 0; i < jsonLocMateRelation.size(); i++) {
			JSONObject object = jsonLocMateRelation.getJSONObject(i);
			ArrayList<String> item = new ArrayList<String>();
			item.add(null);
			item.add(null);
			item.add(null);
			item.set(0, object.getString("weight"));
			mapAll.put(object.getString("source"), item);
		}
		for (int i = 0; i < jsonColleagueRelation.size(); i++) {
			JSONObject object = jsonColleagueRelation.getJSONObject(i);
			String name = object.getString("source");
			
			if (mapAll.get(name) == null) {
				ArrayList<String> item = new ArrayList<String>();
				item.add(null);
				item.add(null);
				item.add(null);
				item.set(1, object.getString("weight"));
				mapAll.put(object.getString("source"), item);
			} else {
				ArrayList<String> list = mapAll.get(name);
				list.set(1, object.getString("weight"));
			}
		}
		for (int i = 0; i < jsonSchoolMateRelation.size(); i++) {
			JSONObject object = jsonSchoolMateRelation.getJSONObject(i);
			String name = object.getString("source");
			
			if (mapAll.get(name) == null) {
				ArrayList<String> item = new ArrayList<String>();
				item.add(null);
				item.add(null);
				item.add(null);
				item.set(2, object.getString("weight"));
				mapAll.put(object.getString("source"), item);
			} else {
				ArrayList<String> list = mapAll.get(name);
				list.set(2, object.getString("weight"));
			}
		}
		//mapAll暂时只取前50个数据进行展示，存入mapAllLimited
		Map<String, ArrayList<String>> mapAllLimited = new HashMap<String, ArrayList<String>>();
		int i = 0;
		for (Entry<String, ArrayList<String>> entryAll : mapAll.entrySet()) {
			if (++i > 50)
				break;
			mapAllLimited.put(entryAll.getKey(), entryAll.getValue());
		}
		JSONObject jsonAllRelation = JSONObject.fromObject(mapAllLimited);
		return jsonAllRelation;
	}

	@Override
	// 修改分级信息
	public boolean setLevel(String id, int level) {
		if (profileDao.updateById(id, "level", level) == 1)
			return true;
		else
			return false;
	}

	@Override
	// 获取全部人员的人名列表
	public JSONArray findAllNameList() {
		JSONArray nameList = profileDao.findAllNames();
		JSONArray result = new JSONArray();
		// 对nameList进行处理，原nameList格式为[ {name : ..}, {name : ..} ]
		for (int i = 0; i < nameList.size(); i++) {
			JSONObject item = nameList.getJSONObject(i);
			result.add(item.getString("name"));
		}
		return result;
	}

	@Override
	// 获取各个级别的人员数量，并存入列表
	public JSONArray findRankCount() {
		JSONArray result = new JSONArray();
		
		long count1 = profileDao.countByKey("level", 1);
		long count2 = profileDao.countByKey("level", 2);
		long count3 = profileDao.countByKey("level", 3);
		
		result.add(count1);
		result.add(count2);
		result.add(count3);
		
		return result;
	}

	@Override
	public JSONObject countByWorkProvince(String province) {
		JSONObject obj = new JSONObject();
		long levelOneNum = 0;
		long levelTwoNum = 0;
		long levelThreeNum = 0;
		if (province.equals("北京") || province.equals("上海") || province.equals("天津") || province.equals("重庆")) {
			levelOneNum = profileDao.countByWorkProvinceAndLevel("city", province, 1);
			levelTwoNum = profileDao.countByWorkProvinceAndLevel("city", province, 2);
			levelThreeNum = profileDao.countByWorkProvinceAndLevel("city", province, 3);
		} else {
			levelOneNum = profileDao.countByWorkProvinceAndLevel("province", province, 1);
			levelTwoNum = profileDao.countByWorkProvinceAndLevel("province", province, 2);
			levelThreeNum = profileDao.countByWorkProvinceAndLevel("province", province, 3);
		}
		obj.put("levelOneNum", levelOneNum);
		obj.put("levelTwoNum", levelTwoNum);
		obj.put("levelThreeNum", levelThreeNum);
		return obj;
	}

	@Override
	public long countByCurrentOfficeRank(String rank) {
		return profileDao.countByKey("latestOfficeRecord.rank", rank);
	}

	@Override
	public JSONArray findByWorkProvince(String province) {
		if (province.equals("北京") || province.equals("上海") || province.equals("天津") || province.equals("重庆")) {
			return profileDao.findByWorkProvince("city", province);
		} else {
			return profileDao.findByWorkProvince("province", province);
		}
	}

	@Override
	public long countByWorkProvinceAndTime(String province, Date start, Date end) {
		if (province.equals("北京") || province.equals("上海") || province.equals("天津") || province.equals("重庆")) {
			return profileDao.countByWorkProvinceAndTime("city", province, start, end);
		} else {
			return profileDao.countByWorkProvinceAndTime("province", province, start, end);
		}
	}

	@Override
	public JSONArray findByWorkProvinceAndTime(String province, Date start,	Date end) {
		List<Profile> profileList = null;
		if (province.equals("北京") || province.equals("上海") || province.equals("天津") || province.equals("重庆")) {
			profileList = profileDao.findByWorkProvinceAndTime("city", province, start, end);
		} else {
			profileList = profileDao.findByWorkProvinceAndTime("province", province, start, end);
		}
		// profileList -> JSONArray
		JSONArray result = new JSONArray();
		for (Profile p : profileList) {
			JSONObject obj = new JSONObject();
			obj.put("_id", p.getId());
			obj.put("name", p.getName());
			obj.put("level", p.getLevel());
			result.add(obj);
		}
		return result;
	}

}
