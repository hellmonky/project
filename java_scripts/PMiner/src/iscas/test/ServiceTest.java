package iscas.test;

import java.util.Calendar;
import java.util.Date;

import javax.annotation.Resource;

import iscas.entity.User;
import iscas.service.ProfileService;
import iscas.service.UserService;

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

public class ServiceTest extends AbstractJUnit4SpringContextTests {

	@Resource
	private ProfileService profileService;
	@Resource
	private UserService userService;
	
	@Test
	public void testFindRelation() {
		//fail("Not yet implemented");
		System.out.println("Location Relation: " + profileService.findLocMate("4bf5b729-6744-480c-86d3-2a94b6a7a598"));
		
		System.out.println("Colleague Relation: " + profileService.findColleague("4bf5b729-6744-480c-86d3-2a94b6a7a598"));
		System.out.println("Colleague Relation: " + profileService.findRankColleague("4bf5b729-6744-480c-86d3-2a94b6a7a598"));
		
		System.out.println("Current SchoolMate Relation: " + profileService.findCurrentSchoolMate("4bf5b729-6744-480c-86d3-2a94b6a7a598"));
		System.out.println("PreviousSchoolMate Relation: " + profileService.findPreviousSchoolMate("4bf5b729-6744-480c-86d3-2a94b6a7a598"));
		System.out.println("All     SchoolMate Relation: " + profileService.findAllSchoolMate("4bf5b729-6744-480c-86d3-2a94b6a7a598"));
		
		System.out.println("All Relation: " + profileService.findAllRelation("4bf5b729-6744-480c-86d3-2a94b6a7a598"));
	}

	@Test
	public void testGetAllNames() {
		System.out.println("All Names: " + profileService.findAllNameList());
	}
	
	@Test
	public void testCount() {
		System.out.println("Rank Count:" + profileService.findRankCount());
		
		Calendar calendar = Calendar.getInstance();
		calendar.add(Calendar.HOUR_OF_DAY, -5);
		Date start = calendar.getTime();
		System.out.println("自10点起河南省新加入的简历数量" + profileService.countByWorkProvinceAndTime("河南", start, new Date()));
	}
	
	@Test
	public void testUser() {
		User user = new User("admin", "a@pminer");
		userService.createUser(user);
	}
}
