cor_kendall<-function(r){
##################################################################################
##	Description of Class Function:
##	<Enter ur description here>
##	input: 
##	output: 
##		
##	Note:
##	
## 
##@File          : ${nameAndExt}
##@Created       : <2013.12.09>,${time}
##@Encoding      : ${encoding}
##@Version       : 
##@Author        : <Gaby Lai> <${Email}>
##################################################################################

	### set environment ###
	##nb
	##setwd("C:\\Users\\Gabrielle\\Google ∂≥∫›µw∫–\\Magic Briefcase\\results\\project2\\v2_hc")
	setwd("~/Google Drive/Magic Briefcase/results/project2_hierarchical clustering/pws")
	
	mydata<-read.csv("TO_profile_DOS.csv", header=T, sep=",", row.names = NULL)
	#print(mydata[1,2])
	tau_array = array(0,c(47,2))
	loop = 47
	
	#if(false){
		
	x <- as.numeric(mydata[,1])	
	for(i in 1:loop){
		y <- as.numeric(mydata[,i])
		temp<- cor.test(x,y, method="kendall")
		str<-paste("(1", "vs.", sep=" ")
		str<-paste(str, i, sep=" ")
		str<-paste(str, ")", sep="")
		#print(str)
		tau_array[i,1] = str
		tau_array[i,2] = temp$estimate
		#print(temp$estimate)
	}
	print(tau_array)
	#}
		
}
