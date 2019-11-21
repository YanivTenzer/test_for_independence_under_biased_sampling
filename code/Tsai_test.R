# Tsai's test for quasi-independence. [Tsai, 1990]
# test for truncation independent of total time
# X - truncation time, 
# Y - lifetime (no censoring)
# The criterion of truncation is X<=Y
TsaiTestTies <- function(X,Y){
  ii <- order(Y)
  X.sort <- X[ii]
  Y.sort <- Y[ii]
  n <- length(X)
  S <- V <- rep(0, n)
  for (i in 1:n){
    risk.i <- which(X.sort[i:n]<=Y.sort[i])+i-1
    S[i] <- sum(X.sort[risk.i]>X.sort[i])-sum(X.sort[risk.i]<X.sort[i])
    t.risk <- table(X.sort[risk.i])
    R <- length(risk.i)
    V[i] <- (R^2-1)/3-sum(t.risk^3-t.risk)/(3*R)
  }
  var.H0 <- sum(V[which(is.nan(V)==FALSE)])
  tsai.stat <- sum(S)^2/var.H0
  tsai.p <- 1-pchisq(tsai.stat,1)
  return(c(tsai.stat,tsai.p))
}
