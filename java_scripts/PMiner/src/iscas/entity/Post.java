package iscas.entity;

import org.mongodb.morphia.annotations.Embedded;

@Embedded
public class Post {
	private String name;
	private String rank;
	
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public String getRank() {
		return rank;
	}
	public void setRank(String rank) {
		this.rank = rank;
	}
}
