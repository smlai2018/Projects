nullModel5<-function(r){
	setwd("~/Google Drive/Magic Briefcase/results/project3_v4/PWS/")
	library("picante")
	tree_pws_tot <-read.tree(file="~/Google Drive/Magic Briefcase/results/project3_v4/PWS/phyliptree_pws_tot.phy")
	dist.tr<-cophenetic(tree_pws_tot)
	write.table(dist.tr, "~/Google Drive/Magic Briefcase/results/project3_v4/PWS/cophenetic_pws_tot.txt", sep="\t", row.names=TRUE)
	
	pws_tot_pairdist = read.csv("cophenetic_pws_tot.csv", header=TRUE)
	clusterNum<-c(9,28,37,7)
	out<-matrix(0, nrow=length(clusterNum), ncol=8)
	rownames(out)<- paste("cluster", 1:length(clusterNum), sep='') 
	colnames(out)<-c("ntaxa", "mntd.obs", "mntd.rand.mean", "mntd.rand.sd", "mntd.obs.rank", "mntd.obs.z", "mntd.obs.p", "runs")
		
	#obs
	library("picante")
	tree41<-read.tree(file="~/Google Drive/Magic Briefcase/results/project3_v4/PWS/Pws_phyliptree_cluster4_1.phy")
	tree42<-read.tree(file="~/Google Drive/Magic Briefcase/results/project3_v4/PWS/Pws_phyliptree_cluster4_2.phy")
	tree43<-read.tree(file="~/Google Drive/Magic Briefcase/results/project3_v4/PWS/Pws_phyliptree_cluster4_3.phy")
	tree44<-read.tree(file="~/Google Drive/Magic Briefcase/results/project3_v4/PWS/Pws_phyliptree_cluster4_4.phy")
	#obs.mean
	b<-cophenetic(tree41)
	diag(b)<-NA
	out[1,2]<-mean(apply(b,2, min, na.rm=TRUE))
	c<-cophenetic(tree42)
	diag(c)<-NA
	out[2,2]<-mean(apply(c,2, min, na.rm=TRUE))
	d<-cophenetic(tree43)
	diag(d)<-NA
	out[3,2]<-mean(apply(d,2, min, na.rm=TRUE))
	e<-cophenetic(tree44)
	diag(e)<-NA
	out[4,2]<-mean(apply(e,2, min, na.rm=TRUE))
	
	runs=100
	for(i in 1:(length(clusterNum))){
		#ntaxa
		out[i,1]<- clusterNum[i]
		#runs
		out[i,8] <- runs
	}
	#cat("out: ", out, "\n")
	
	# mntd of randomized tree
	mntd.out.r<-matrix(0, nrow=length(clusterNum), ncol= runs, byrow=TRUE)
	for(i in 1:runs){
		samp_idxs<-sample(nrow(pws_tot_pairdist), size=nrow(pws_tot_pairdist), replace=FALSE)
		#cat("samp_idxs: ", samp_idxs, "\n")
		iidx=1
		for(j in 1:(length(clusterNum))){
			iidx_end=iidx+clusterNum[j]-1
			cidx=c(samp_idxs[iidx:iidx_end])
			#cat("iidx: ", iidx, " iidx_end: ", iidx_end, " cidx: ", cidx, "\n")
			mysample<-pws_tot_pairdist[cidx, cidx]
			#print(mysample)
			diag(mysample)<-NA
			mntd.out.r[j,i]<-mean(apply(mysample,2,min,na.rm=TRUE))
			iidx=iidx_end+1
		}	
	}
	cat(mntd.out.r, "\n")
	
	par(mfrow=c(2,2))
	c1<-mntd.out.r[1,]
	c2<-mntd.out.r[2,]
	c3<-mntd.out.r[3,]
	c4<-mntd.out.r[4,]
	hist(c1, main="Histogram for PWS-c1", xlab="phylogenetic mean of random-100 runs")
	hist(c2, main="Histogram for PWS-c2", xlab="phylogenetic mean of random-100 runs")
	hist(c3, main="Histogram for PWS-c3", xlab="phylogenetic mean of random-100 runs")
	hist(c4, main="Histogram for PWS-c4", xlab="phylogenetic mean of random-100 runs")
	
	for(i in 1:(length(clusterNum))){
		rand.mean<- mean(mntd.out.r[i,])
		out[i,3] <- rand.mean
		#rand.sd
		rand.sd<- sd(mntd.out.r[i,])
		out[i,4] <- rand.sd
		#obs.rank
		obs.rank <- rank(c(out[i,2], mntd.out.r[i,]))[1]
		out[i,5]<-obs.rank
		#out.z
		out.z = (out[i,2]-rand.mean) / rand.sd
		out[i,6]<-out.z
		#obs.p  #calculation of probability of the observed MNTD
		mntd.obs.p <- obs.rank/(runs+1)
		out[i,7]<-mntd.obs.p
	}
}