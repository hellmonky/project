package iscas.test;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.annotation.Resource;

import iscas.dao.ProfileDao;
import iscas.entity.Profile;

import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit4.AbstractJUnit4SpringContextTests;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;

//import static org.junit.Assert.*;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(SpringJUnit4ClassRunner.class)

@ContextConfiguration(
	locations = {
		"classpath:beansContext.xml",
	}
)

public class MongodbTest extends AbstractJUnit4SpringContextTests {
	
	@Resource
	private ProfileDao profileDao;

	@Test
	public void testFind() {
		//assertTrue
		//assertFalse
		//assertEquals
		//assertNotNull
		//assertNull
		//assertSame
		//assertNotSame
		//fail
		Profile p = profileDao.findByName("王成文").get(0);
		System.out.println(p.getName() + ":" + p.getAge());
		
		List<Profile> list = profileDao.findByKey("birthProvince", "辽宁");
		System.out.println(list.size());
		
		list = profileDao.findByKeyStartsWith("birthDate", "1962");
		System.out.println(list.size());
	}
	
	@Test
	public void testStatistics() {
		System.out.println("总人员数量: " + profileDao.getCount());
		// BirthPlace
		System.out.println("出生地在陕西的人员数量： " + profileDao.countByKey("birthProvince", "陕西"));
		System.out.println("出生地在河南的人员数量： " + profileDao.countByKey("birthProvince", "河南"));
		System.out.println("出生地在北京的人员数量： " + profileDao.countByKey("birthProvince", "北京"));
		// OfficeRecord
		System.out.println("在湖南工作过的人员数量： " + profileDao.countByKey("officeRecord.province", "湖南"));
	}
	
	@Test
	public void testUpdate() {
		//System.out.println("更新数量：" + profileDao.updateById(2L, "age", "55"));
	}
	
	@Test
	public void testSave() {
		// Saves the entity; either inserting or overriding the existing document
		Profile p = profileDao.findByName("韩永文").get(0);
		System.out.println("原年龄：" + p.getAge());
		p.setAge("60");
		System.out.println(profileDao.saveOrModify(p));
		
		Profile re = profileDao.findByName("韩永文").get(0);
		System.out.println("现年龄：" + re.getAge());
	}
	
	@Test
	public void testFindRelation() {
		System.out.println(profileDao.findLocMate("河南").toString());
		System.out.println(profileDao.findColleague("北京大学").toString());
		System.out.println(profileDao.findSchoolMate("北京大学").toString());
	}
	
	@Test
	public void testDelete() {
		/*
		if (profileDao.deleteProfile("4bf5b729-6744-480c-86d3-2a94b6a7a598"))
			System.out.println("Delete id 325 Succeed!");
		else
			System.out.println("Delete id 325 Failed!");
		*/
	}
	
	@Test
	public void testList() {
		List<String> list = new ArrayList<String>();
		list.add("b1");
		list.add("b2");
		list.add("b3");
		list.add("b4");
		list.add("b5");
		System.out.println("删除前的集合为:" + list);
		System.out.println("删除前集合的长度为:" + list.size());
		Iterator<String> it=list.iterator();
		while(it.hasNext()){
			String temp = it.next();
			if (temp.equals("b1"))
				it.remove();
		}
		System.out.println("删除后的集合为:" + list);
		System.out.println("删除后集合的长度为:" + list.size());
	}
	
	@Test
	public void testProvince() {
		System.out.println("陕西省的人数： " + profileDao.findByWorkProvince("province", "陕西").size());
		System.out.println("北京市的人数： " + profileDao.findByWorkProvince("city", "北京").size());
	}
	
	@Test
	public void testWrokRankCount() {
		System.out.println("正国级的人数： " + profileDao.countByKey("latestOfficeRecord.rank", "10"));
		System.out.println("副国级的人数： " + profileDao.countByKey("latestOfficeRecord.rank", "9"));
	}

}
