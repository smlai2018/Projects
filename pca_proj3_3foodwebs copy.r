pca_proj3_3foodwebs<-function(r){
##################################################################################
## Description of Class Function:
## <Enter ur description here>
## input: network file(.TXT)
## output: a pca result of a food web
## 
##Note:
##<Enter ur note here>
##
##@File          : ${nameAndExt}
##@Created       : <2014.12.04>,${time}
##@Encoding      : ${encoding}
##@Version       : <V1>
##@Author        : <Gaby Lai> <${Email}>
##################################################################################
	#set environment
	setwd("~/Google Drive/Magic Briefcase/results/project3")
	#setwd("C://Users//Gabrielle//Google ¶³ºÝµwºÐ//Magic Briefcase//results//project3")
	foodweb = "rand"
    data_bioNnw<-read.csv(file=paste(foodweb, "_pca_input.csv", sep=""), header=T, sep=",", row.names=1)
    data_bio<-read.csv(file=paste(foodweb, "_pca_biotic input.csv", sep=""), header=T, sep=",", row.names=1)
    data_nw<-read.csv(file=paste(foodweb, "_pca_nw input.csv", sep=""), header=T, sep=",", row.names=1)
    pca1_bioNnw<-princomp(data_bioNnw, cor=T)
    pca1_bio<-princomp(data_bio, cor=T)
    pca1_nw<-princomp(data_nw, cor=T)
    

    #print(pca1$loadings, digits=8, cutoff=0)
    #pca1$scores
    #plot(pca1, type="l", main="PCA plot using PWS data(biotic and network indices)")
    #biplot(pca1)
    
    # PWS functional groups
	#g <-c(1,1,3,10,1,1,10,4,10,2,4,10,8,8,4,4,7,1,2,4,5,9,1,6,9,9,4,6,8,9,9,5,6,11,11,6,6,6,6,6,11,11)
	# NCC functional groups
	#g<-c(6,11,6,11,11,11,11,5,5,6,6,6,6,5,9,9,4,4,9,2,2,2,3,3,4,7,7,7,7,7,7,7,7,7,7,7,7,4,10,10,10,10,10,10,10,10,2,3,8,8,8,1,1,1,1,1,1,1,1)
	# NBC functional groups
	#g<-c(1,1,1,1,8,2,2,2,5,5,3,3,4,4,9,9,9,9,7,7,7,7,7,7,7,10,10,10,10,10,10,4,4,4,4,4,4,4,3,6,6,6,6,6,6,5,11,11,6)
	# SEA functional groups
	#g<-c(1,1,1,1,1,1,1,1,1,8,3,3,2,2,9,9,9,4,4,7,7,7,4,4,10,10,4,10,4,4,4,6,6,6,5,11,11)
	
    ari = matrix(nrow=10, ncol=7)
    
    # K-means clustering
	# Determine the correct number of clusters via weighted sum of squares
	# (from R in Action: http://www.statmethods.net/advstats/cluster.html)
	
	#wss <- (nrow(comp)-1)*sum(apply(comp,2,var))
	#for (i in 2:15) wss[i] <- sum(kmeans(comp, centers=i, nstart=100, iter.max=1000)$withinss)
	#plot(1:15, wss, type="b", xlab="Number of Clusters", ylab="Within groups sum of squares")
    
    sink("rand_comp5_output.txt", append=FALSE, split=FALSE)
    
	foodweb_list <- c("pws1","ncc","nbc", "sea","rand")
	foodweb_index = grep(foodweb,foodweb_list)
	print(foodweb_index)
	print("\n")
	
	######## ARI #########
    # From scree plot elbow occurs at k = 4
	# Apply k-means with k=4  
	library("fossil")
	
    ifoodweb=0
    while(ifoodweb <3){
    	print(ifoodweb)
    	print("\n")
    	if (ifoodweb == 0){
    		comp<-data.frame(pca1_bioNnw$scores[,1:5])
    	}
    		
		else if(ifoodweb == 1){
			comp<-data.frame(pca1_bio$scores[,1:5])
		}
			
		else{
			comp<-data.frame(pca1_nw$scores[,1:5])
		}
		print(comp)
		print("\n")
			
		groups=11
		while(groups > 4){
			iloops=10
		
			if(foodweb_index == 2)	#NCC
				g<-c(6,11,6,11,11,11,11,5,5,6,6,6,6,5,9,9,4,4,9,2,2,2,3,3,4,7,7,7,7,7,7,7,7,7,7,7,7,4,10,10,10,10,10,10,10,10,2,3,8,8,8,1,1,1,1,1,1,1,1)
			else if(foodweb_index == 3)	#NBC
				g<-c(1,1,1,1,8,2,2,2,5,5,3,3,4,4,9,9,9,9,7,7,7,7,7,7,7,10,10,10,10,10,10,4,4,4,4,4,4,4,3,6,6,6,6,6,6,5,11,11,6)
			else if(foodweb_index == 4)	#SEA
				g<-c(1,1,1,1,1,1,1,1,1,8,3,3,2,2,9,9,9,4,4,7,7,7,4,4,10,10,4,10,4,4,4,6,6,6,5,11,11)
			else	#PWS1, rand
				g<-c(1,1,3,10,1,1,10,4,10,2,4,10,8,8,4,4,7,1,2,4,5,9,1,6,9,9,4,6,8,9,9,5,6,11,11,6,6,6,6,6,11,11)

		
			while ( iloops >0 ){
				k <- kmeans(comp, groups, nstart=25, iter.max=1000)
				g = rbind(g, k$clust)
				
				iloops = iloops - 1
			}	#while ( iloops >0 )
			print(g)
			print("\n")
		
			iloops=10
			i=2
			while( i <= (iloops+1) ){	
				ari[i-1,12-groups] = adj.rand.index(g[1,],g[i,])
				i = i + 1	
			}	# while( i < iloops )	
		
			groups = groups - 1
		}	#	while(k>4)
	
		print(ari)
		print("\n")
		
		ifoodweb = ifoodweb+1
	}	#while(ifoodweb <3)
	
	sink()
		
}