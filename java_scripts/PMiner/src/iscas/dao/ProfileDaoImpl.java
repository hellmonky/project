package iscas.dao;

import java.util.Date;
import java.util.List;

import iscas.entity.Profile;

import com.mongodb.BasicDBObject;
import com.mongodb.DBCollection;
import com.mongodb.DBObject;
import com.mongodb.MongoClient;
import com.mongodb.WriteResult;

import net.sf.json.JSONArray;

import org.mongodb.morphia.Key;
import org.mongodb.morphia.Morphia;
import org.mongodb.morphia.dao.BasicDAO;
import org.mongodb.morphia.query.Query;
import org.mongodb.morphia.query.UpdateOperations;
import org.mongodb.morphia.query.UpdateResults;

public class ProfileDaoImpl extends BasicDAO<Profile, String> implements ProfileDao {
	
	private final static int limit = 50;

	protected ProfileDaoImpl(MongoClient mongoClient, Morphia morphia,
			String dbName) {
		super(mongoClient, morphia, dbName);
		// TODO Auto-generated constructor stub
	}

	@Override
	public long getCount() {
		return count();
	}

	@Override
	public Profile findById(String id) {
		return get(id);
	}
	
	@Override
	public List<Profile> findByKey(String key, String value) {
		Query<Profile> query = createQuery().field(key).equal(value);
		List<Profile> list = find(query).asList();
		return list;
	}
	
	@Override
	public List<Profile> findByName(String name) {
		return findByKey("name", name);
	}
	
	@Override
	public List<Profile> findByKeyStartsWith(String key, String value) {
		Query<Profile> query = createQuery().field(key).startsWith(value);
		List<Profile> list = find(query).asList();
		return list;
	}
	
	@Override
	public long countByKey(String key, Object value) {
		return count(key, value);
	}

	@Override
	// 返回更新记录的数量，对于按Id更新，返回值只能为0或1
	public int updateById(String id, String key, Object value) {
		UpdateOperations<Profile> ops = createUpdateOperations().set(key, value);
		Query<Profile> query = createQuery().field("_id").equal(id);
		
		UpdateResults re = update(query, ops);
		return re.getUpdatedCount();
	}

	@Override
	// 保存或覆盖Profile实体对象
	public String saveOrModify(Profile profile) {
		// returns the Id of profile
		Key<Profile> result = this.save(profile);
		return (String) result.getId();
	}

	@Override
	public boolean deleteProfile(String id) {
		WriteResult re = this.deleteById(id);
		if (re.getN() == 1)
			return true;
		else
			return false;
	}

	@Override
	// 查找同乡关系
	public JSONArray findLocMate(String province) {
		DBCollection collection = this.getCollection();
		BasicDBObject queryObject = new BasicDBObject();
		queryObject.put("birthProvince", province);
		
		BasicDBObject returnFields = new BasicDBObject();
		returnFields.put("name", 1);
		returnFields.put("birthProvince", 1);
		returnFields.put("birthCity", 1);
		returnFields.put("birthDistrict", 1);

		List<DBObject> objs = collection.find(queryObject, returnFields).limit(limit).toArray();
		
		return JSONArray.fromObject(objs);
	}

	@Override
	public JSONArray findColleague(String unitName) {
		DBCollection collection = this.getCollection();
		BasicDBObject queryObject = new BasicDBObject();
		queryObject.put("officeRecord.tupleList.unitNameList", unitName);
		
		BasicDBObject returnFields = new BasicDBObject();
		returnFields.put("name", 1);
		returnFields.put("officeRecord.$", 1);
		
		List<DBObject> objs = collection.find(queryObject, returnFields).limit(limit).toArray();
		
		return JSONArray.fromObject(objs);
	}

	@Override
	public JSONArray findSchoolMate(String universityName) {
		DBCollection collection = this.getCollection();
		BasicDBObject queryObject = new BasicDBObject();
		queryObject.put("studyRecord.universityName", universityName);
		
		BasicDBObject returnFields = new BasicDBObject();
		returnFields.put("name", 1);
		returnFields.put("studyRecord.$", 1);

		List<DBObject> objs = collection.find(queryObject, returnFields).limit(limit).toArray();
		
		return JSONArray.fromObject(objs);
	}
	
	// 以下是一些辅助方法
	// 获取所有用户的姓名列表
	@Override
	public JSONArray findAllNames() {
		DBCollection collection = this.getCollection();
		
		BasicDBObject returnFields = new BasicDBObject();
		returnFields.put("name", 1);
		returnFields.put("_id", 0);

		List<DBObject> objs = collection.find(new BasicDBObject(), returnFields).toArray();
		
		return JSONArray.fromObject(objs);
	}

	@Override
	// 根据当前工作的省市信息获取所有人的id和姓名
	public JSONArray findByWorkProvince(String provinceOrCity, String provinceName) {
		DBCollection collection = this.getCollection();
		DBObject projection = new BasicDBObject().append("_id", 1).append("name", 1).append("level", 1);
		
		List<DBObject> objs = collection.find(new BasicDBObject().append("latestOfficeRecord." + provinceOrCity, provinceName), projection).toArray();
		
		JSONArray result = new JSONArray();
		for (DBObject obj : objs) {
			result.add(obj);
		}
		return result;
		
		/**
		 * 在没有保存最近工作记录的情况下，可以使用MongoDB支持的聚集方式查询
		
		List<DBObject> pipeline = new ArrayList<DBObject>();
		Pattern pattern = Pattern.compile("^2014");
		
		DBObject match = new BasicDBObject().append("$match", new BasicDBObject().append("officeRecord." + provinceOrCity, provinceName));
		DBObject project = new BasicDBObject().append("$project", new BasicDBObject().append("name", 1).append("officeRecord", 1).append("level", 1));
		DBObject unwind = new BasicDBObject().append("$unwind", "$officeRecord");
		DBObject group = new BasicDBObject().append("$group", new BasicDBObject().append("_id", "$_id").append("latestOfficeRecord", new BasicDBObject().append("$last", "$officeRecord")).append("name", new BasicDBObject().append("$last", "$name")).append("level", new BasicDBObject().append("$last", "$level")));
		DBObject match2 = new BasicDBObject().append("$match", new BasicDBObject().append("latestOfficeRecord." + provinceOrCity, provinceName).append("latestOfficeRecord.endDate", pattern));
		pipeline.add(match);
		pipeline.add(project);
		pipeline.add(unwind);
		pipeline.add(group);
		pipeline.add(match2);
		
		JSONArray result = new JSONArray();
		for (DBObject object : collection.aggregate(pipeline).results()) {
			JSONObject item = new JSONObject();
			item.put("id", object.get("_id"));
			item.put("name", object.get("name"));
			item.put("level", object.get("level"));
			result.add(item);
		}
		return result;
		 */
	}

	@Override
	public long countByWorkProvinceAndLevel(String provinceOrCity, String provinceName, int level) {
		Query<Profile> query = createQuery().field("latestOfficeRecord." + provinceOrCity).equal(provinceName);
		query.field("level").equal(level);
		return this.count(query);
	}

	@Override
	public List<Profile> findByTime(Date start, Date end) {
		Query<Profile> query = createQuery().field("timestamp").greaterThanOrEq(start);
		query.field("timestamp").lessThanOrEq(end);
		return this.find(query).asList();
	}

	@Override
	public long countByWorkProvinceAndTime(String provinceOrCity, String provinceName, Date start, Date end) {
		Query<Profile> query = createQuery().field("latestOfficeRecord." + provinceOrCity).equal(provinceName);
		query.field("timestamp").greaterThanOrEq(start);
		query.field("timestamp").lessThanOrEq(end);
		return this.count(query);
	}

	@Override
	public List<Profile> findByWorkProvinceAndTime(String provinceOrCity, String provinceName, Date start, Date end) {
		Query<Profile> query = createQuery().field("latestOfficeRecord." + provinceOrCity).equal(provinceName);
		query.field("timestamp").greaterThanOrEq(start);
		query.field("timestamp").lessThanOrEq(end);
		return this.find(query).asList();
	}

}
