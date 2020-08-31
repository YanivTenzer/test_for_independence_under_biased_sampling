#include <iostream>
#include <math.h>
// #include <map>
#include <random>

// #include <Rcpp.h> // for including R with Rcpp
#include <RcppArmadillo.h>
// [[Rcpp::depends(RcppArmadillo)]]

// #include "utilities_ToR.h"


using namespace std;
using namespace Rcpp;  // for including R with Rcpp 

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp
// function (or via the Source button on the editor toolbar). ...
// [[Rcpp::export]]
int timesEight(int x) {
	return x * 8;
}

// [[Rcpp::export]]
int plusFive(int x) {
	return x + 5;
}

// [[Rcpp::export]]
Rcpp::NumericVector add_one_sqrt(NumericVector x) {
	NumericVector y(x.length()); //	y(x);
	y = sqrt(x + 1.0);
	return y;
}

/**
// [[Rcpp::export]]
arma::vec arma_sort(arma::vec x, arma::vec y) {
	return x(arma::sort_index(y));
}
**/

// [[Rcpp::export]]
NumericVector stl_sort(NumericVector x) {
	NumericVector y = clone(x);
	std::sort(y.begin(), y.end());
	return y;
}


/**
// [[Rcpp::export(".mm")]]
arma::mat mm_mult(const arma::mat& lhs,
	const arma::mat& rhs)
{
	return lhs * rhs;
}
// [[Rcpp::export(".vm")]]
arma::mat vm_mult(const arma::vec& lhs,
	const arma::mat& rhs)
{
	return lhs.t() * rhs;
}
// [[Rcpp::export(".mv")]]
arma::mat mv_mult(const arma::mat& lhs,
	const arma::vec& rhs)
{
	return lhs * rhs;
}
// [[Rcpp::export(".vv")]]
arma::mat vv_mult(const arma::vec& lhs,
	const arma::vec& rhs)
{
	return lhs.t() * rhs;
}
**/



// [[Rcpp::export]]
NumericVector my_mv_mult(NumericMatrix A, NumericVector V)
{
	NumericVector r(A.nrow());
	long i, j;

	for (i = 0; i < A.nrow(); i++)
	{
		r[i] = 0.0;
		for (j = 0; j < A.ncol(); j++)
			r[i] += A(i, j) * V[j];
	}
	return(r);
}



// Hoeffding's test in Rcpp

// Get indices of vector sorted 
// [[Rcpp::export]]
IntegerVector sort_indexes_rcpp(NumericVector data)
{

	IntegerVector index = seq_along(data) - 1;

	sort(index.begin(), index.end(),
		[&](const int& a, const int& b) {
			return (data[a] < data[b]);
		});
//	cout << "sort perm:" << endl; 
//	for (long i = 0; i < index.length(); i++)
//		cout << index[i] << ", "; 
//	cout << endl;
	return(index);
}

// Gaussian density
double normalPDF(double value)
{
   return exp(-value*value*0.5) /  2.506628; // divide by sqrt(2*pi) 
}


// Gaussian cumulative
double normalCDF(double value)
{
   return 0.5 * erfc(-value * M_SQRT1_2);
}


/** Copied from here: https://stackoverflow.com/questions/37143283/finding-unique-rows-in-armamat **/
template <typename T>
inline bool rows_equal(const T& lhs, const T& rhs, double tol = 0.00000001) {
	return arma::approx_equal(lhs, rhs, "absdiff", tol);
}

// [[Rcpp::export]]
arma::mat unique_rows(const arma::mat& x) {
	unsigned int count = 1, i = 1, j = 1, nr = x.n_rows, nc = x.n_cols;
	arma::mat result(nr, nc);
	result.row(0) = x.row(0);

	for (; i < nr; i++) {
		bool matched = false;
		if (rows_equal(x.row(i), result.row(0))) continue;

		for (j = i + 1; j < nr; j++) {
			if (rows_equal(x.row(i), x.row(j))) {
				matched = true;
				break;
			}
		}

		if (!matched) result.row(count++) = x.row(i);
	}

	return result.rows(0, count - 1);
}
/**/

// [[ Rcpp :: export ()]]
//NumericMatrix a4(arma::mat x) {
//	NumericMatrix y = wrap(x);
//	return(y);
//}


/*
* Copied from here: https://figshare.com/articles/Algorithm_of_binary_search_Rcpp_code/3394741
* A binary search divides a range of values (a sorted array) into halves by the median point(noted k), and continues
 * tonarrow down the field of search until the pattern(noted x) is found.
 * It is the classic example of a "divide and conquer" algorithm.
 */
 // [[Rcpp::export]]
int binary_search_rcpp(NumericVector array, double pattern, long exact_match=0) {
	int array_length = array.size();
	int i = 0, j = array_length - 1;
	int k;
	while (i < j) {
		k = (i + j) / 2;
		if (array[k] == pattern) {
			return k; // return index 
		}
		else if (array[k] < pattern) {
			i = k+1;
		}
		else {
			j = k;
		}
	}
	if(exact_match)
		return -1; // didn't find value 
	else
		return i; // return closest
}


// Compute empirical cdf 
// [[Rcpp::export]]
NumericVector ecdf_rcpp(NumericVector x)  
{
	long i;
	long n = x.length();
	IntegerVector Px(n); 
	NumericVector ecdf(n);

	Px = sort_indexes_rcpp(x); 
	for (i = 0; i < n; i++)
		ecdf[Px[i]] = double(i+1) / double(n); // use double division here
	return(ecdf);
}



// Compute one-dimensional empirical cdf
// [[Rcpp::export]]
double ecdf1_rcpp(double x, NumericVector cdf, NumericVector data)
{
	long i, i_max=-1; 
	for(i=0; i<data.length(); i++)
		if(data[i] <= x)
			if((i_max == -1) || (data[i] >= data[i_max]))
				i_max = i;
	if(i_max == -1)
    	return(0.0);
	else
	  return(cdf[i_max]);  
}


// Compute empirical cdf 2d
// [[Rcpp::export]]
double ecdf2_rcpp(NumericVector xy, NumericMatrix cdf_2d, NumericMatrix data)
{
	long i, i_max=-1, j_max=-1;

///	Rcout << "Start loop ecdf2 len=" << data.nrow() << endl; 
	for(i=0; i<data.nrow(); i++)
	{
		if(data(i,0) <= xy[0])
			if((i_max == -1) || (data(i,0) >= data(i_max,0)))
				i_max = i;
		if(data(i,1) <= xy[1])
			if((j_max == -1) || (data(i,0) >= data(j_max,0)))
				j_max = i;
	}

///	Rcout << "End loop ecdf2 i_max=" << i_max << " j_max=" << j_max << endl; 
///	Rcout << "cdf_2d dims=" << cdf_2d.nrow() << ", " << cdf_2d.ncol() << endl; 
	if( (i_max==-1) || (j_max==-1))
    	return(0.0);
	else
  		return(cdf_2d(i_max,j_max));
}


// Compute matrix empirical cdf 2d from just values
/**
NumericVector ecdf2_rcpp(NumericVector x, NumericVector y)
{
	long i, j;
	long n = x.length();
	IntegerVector Px(n), Py(n); 
	NumericMatrix ecdf2(n, n);

	Px = sort_indexes_rcpp(x); 
	for (i = 0; i < n; i++)
		for(j = 0; j < n; j++)
			ecdf[Px[i]][Py[j]] = double(i+1) / double(n); // use double division here
	return(ecdf2);
}
**/


// should have a similar function for a vector !! 
// [[Rcpp::export]]
double w_fun_eval_rcpp(double x, double y, string w_fun)
{
	double r = 0.0;

	// allow only string for now 
	// different bias functions (instead of switch)
	if ((w_fun == "truncation") || (w_fun == "Hyperplane_Truncation"))
		r = x <= y; 
	if ((w_fun == "exp") || (w_fun == "exponent_minus_sum_abs") || (w_fun == "stritcly_positive"))
		r = exp((-abs(x) - abs(y)) / 4);
	if (w_fun == "gaussian")
		r = exp((-x*x - y*y) / 2);
	if (w_fun == "huji")
		r = fmax(fmin(65.0 - x - y, 18.0), 0.0);
	if (w_fun == "sum")
		r = x + y; 
	if ((w_fun == "naive") || (w_fun == "const"))
		r = 1;

	return(r);
}

// [[Rcpp::export]]
NumericVector w_fun_eval_vec_rcpp(NumericVector x, double y, string w_fun)
{
	long n = x.length();
	long i = 0;
	NumericVector r(n);

	for (i = 0; i < n; i++)
	{
		// allow only string for now 
		// different bias functions (instead of switch)
		if ((w_fun == "truncation") || (w_fun == "Hyperplane_Truncation"))
		{
			r[i] = x[i] < y; continue;
		}
		if ((w_fun == "exp") || (w_fun == "exponent_minus_sum_abs") || (w_fun == "stritcly_positive"))
		{
			r[i] = exp((-abs(x[i]) - abs(y)) / 4); continue;
		}
		if (w_fun == "gaussian")
			r[i] = exp((-x[i]*x[i] - y*y) / 2);
		if (w_fun == "huji")
		{
			r[i] = fmax(fmin(65.0 - x[i] - y, 18.0), 0.0); continue;
		}
		if (w_fun == "sum")
		{
			r[i] = x[i] + y; continue;
		}
		if ((w_fun == "naive") || (w_fun == "const"))
		{
			r[i] = 1.0; continue;
		}
	}
	return(r);
}



// [[Rcpp::export]]
NumericMatrix w_fun_to_mat_rcpp(NumericMatrix data, string w_fun)
{
	long n = data.nrow();  // get sample size
	NumericMatrix w_mat(n, n);
	long i; //  , j;
	for (i = 0; i < n; i++)
	{
//		Rcout << "Run VEC!" << endl; 
//		for (j = 0; j < n; j++)
//			w_mat(i, j) = w_fun_eval_rcpp(data(i, 0), data(j, 1), w_fun);
		w_mat(_, i) = w_fun_eval_vec_rcpp(data(_, 0), data(i, 1), w_fun); // fix transform bug 
	}
	return (w_mat);
}


// Determine empirically  w_max for functions where we don't know it (this is approximate and may fail)
/**
double set_w_max_rcpp(double n = 1000, string dependence_type, string w_fun)
{
	w_fun_	
	NumericMatrix xy = SimulateSample_rcpp(n, dependence_type); // need to implement simulate sample 
	return(5 * (max(w_fun_eval_vec_rcpp(xy(_, 1), xy(_, 2), w_fun)) + 5 * std(w_fun_eval_vec_rcpp(xy(_, 1), xy(_, 2), w_fun)) + 1.0));
}
**/


// Set the maximum w value we see in the sample (useful for bootstrap)
// [[Rcpp::export]]
double set_w_max_rcpp_sample(NumericMatrix data, string w_fun)
{
	NumericMatrix w_mat = w_fun_to_mat_rcpp(data, w_fun);
	return max(w_mat); 
}



// Determine if weighing function is positive (doesn't support functions)
// [[Rcpp::export]]
long is_pos_w_rcpp(string w_fun, NumericMatrix data, long mat_flag) // , data, mat.flag)
{
	long i; 
	string pos_w[6] = {"sum", "sum_coordinates", "exponent_minus_sum_abs", "const", "naive", "gaussian"}; // all positive w -> should replace by a function checking for positivity 
	string zero_w[2] = {"truncation", "Hyperplane_Truncation"}; // all positive w -> should replace by a function checking for positivity 

	for(long i=0; i<6; i++)
		if(w_fun == pos_w[i])
			return(TRUE);
	for(long i=0; i<2; i++)
		if(w_fun == zero_w[i])
			return(FALSE);

 	if(!mat_flag)  // run and compute values 
	{
		for(i=0; i < data.nrow(); i++)
		    if(w_fun_eval_rcpp(data(i,0), data(i,1), w_fun ) == 0) 
				return(FALSE);
		return(TRUE);
	} else
	{
		return(min(w_fun_to_mat_rcpp(data, w_fun)) > 0); //  test all pairs x_i, y_j 
	}	
}




//##################################################################################################
// Compute the modified Hoeffding's test statistic, for the permutation test
// Parameters:
// n - number of samples
// data - n * 2 matrix with(x, y) sample
// grid_points - all possible(x_i, y_j) points.size
// null.expectations.table - 4 * n mass table with pre - computed mass estimates
// 
//  Quardants convension :
//   4 | 1
//   ------
//   3 | 2
//##################################################################################################
// [[Rcpp::export]]
double ComputeStatistic_rcpp(NumericMatrix data, NumericMatrix grid_points, NumericMatrix null_expectations_table)
{
	long n = data.nrow();
	double Obs[4] = { 0 };
	double Exp[4] = { 0 };
	long i, j;
	double Statistic = 0.0;
	long* Rx = new long[n];
	long* Ry = new long[n];
	long* Eqx = new long[n];
	long* Eqy = new long[n];

	// New: enable not to give a grid_points as input: set it as data 
	if(grid_points.nrow()<= 1)	
		grid_points = data; 

	for (i = 0; i < n; i++) // Slow loop on grid points
	{
		for (j = 0; j < 4; j++) // loop on quadrants
		{
			Exp[j] = null_expectations_table(i, j);
			Obs[j] = 0;
		}
		for (j = 0; j < n; j++)  // loop on data points  
		{
			Rx[j] = data(j, 0) > grid_points(i, 0);
			Ry[j] = data(j, 1) > grid_points(i, 1);
			Eqx[j] = data(j, 0) == grid_points(i, 0);
			Eqy[j] = data(j, 1) == grid_points(i, 1);
			Obs[0] += double(Rx[j] * Ry[j]);
			Obs[1] += (Rx[j]-Eqx[j]);
			Obs[3] += (Ry[j]-Eqy[j]);
			Obs[2] += (-Eqx[j] - Eqy[j] + Eqx[j] * Eqy[j]);
		}
		Obs[1] -= Obs[0];
		Obs[3] -= Obs[0];
		Obs[2] += (n - Obs[0] - Obs[1] - Obs[3]);  // here need to check the total number of observed! 

		if ((Exp[0] > 1) && (Exp[1] > 1) && (Exp[2] > 1) && (Exp[3] > 1))
			for (j = 0; j < 4; j++)
				Statistic += pow((Obs[j] - Exp[j]), 2) / Exp[j];  // set valid statistic when expected is 0 or very small
	} // end loop on grid points

//	cout << "Return Stat: " << Statistic << endl;
	return(Statistic);
}


// Weighted statistic - alternative version (works only for positive w)
// [[Rcpp::export]]
double ComputeStatistic_w_rcpp(NumericMatrix data, NumericMatrix grid_points, string w_fun) 
{
	long n = data.nrow();
	long i, j;
	NumericMatrix w_vec(n); 
	double n_w=0.0;
	double Statistic = 0.0;
	IntegerVector Rx(n);
	IntegerVector Ry(n);
	IntegerVector Lx(n);
	IntegerVector Ly(n);

	// New: enable not to give a grid_points as input: set it as data 
	if (grid_points.nrow() <= 1)
		grid_points = data;

	for (i = 0; i < n; i++)
	{
		w_vec[i] = w_fun_eval_rcpp(data(i, 0), data(i, 1), w_fun);
		n_w += 1.0 / w_vec[i];
	}
	double Obs[4] = { 0 };
	double Exp[4] = { 0 };

	double Rx_sum, Ry_sum, Lx_sum, Ly_sum; //  Rx_not_sum, Ry_not_sum;

	for (i = 0; i < n; i++) // Slow loop on grid points
	{
		Rx_sum = Ry_sum = Lx_sum = Ly_sum = 0.0; //  Rx_not_sum = Ry_not_sum = 0.0;
		for(j = 0; j < 4; j++)
			Obs[j] = 0;
		for (j = 0; j < n; j++)  // loop on data points  
		{
			Rx[j] = data(j, 0) > grid_points(i, 0);
			Ry[j] = data(j, 1) > grid_points(i, 1);
			Lx[j] = data(j, 0) < grid_points(i, 0);
			Ly[j] = data(j, 1) < grid_points(i, 1);

			Rx_sum += Rx[j] / w_vec[j];
			Ry_sum += Ry[j] / w_vec[j];
			Lx_sum += Lx[j] / w_vec[j];
			Ly_sum += Ly[j] / w_vec[j];

//			Rx_not_sum += (1-Rx[j]) / w_vec[j];
//			Ry_not_sum += (1-Ry[j]) / w_vec[j];


			Obs[0] += (Rx[j] * Ry[j] / (w_vec[j] * n_w));
			Obs[1] += (Rx[j] * Ly[j] / (w_vec[j] * n_w));
			Obs[2] += (Lx[j] * Ly[j] / (w_vec[j] * n_w));
			Obs[3] += (Lx[j] * Ry[j] / (w_vec[j] * n_w));
		}
		Exp[0] = Rx_sum * Ry_sum / (n_w * n_w);
		Exp[1] = Rx_sum * Ly_sum / (n_w * n_w);
		Exp[2] = Lx_sum * Ly_sum / (n_w * n_w);
		Exp[3] = Lx_sum * Ry_sum / (n_w * n_w);

		if ((Exp[0] > 1.0/n) && (Exp[1] > 1.0 / n) && (Exp[2] > 1.0 / n) && (Exp[3] > 1.0 / n))
			for (j = 0; j < 4; j++)
				Statistic += pow((Obs[j] - Exp[j]), 2) / Exp[j];  // set valid statistic when expected is 0 or very small
	}

	return(Statistic);
}


// [[Rcpp::export]]
List GetNullDistribution_rcpp(NumericMatrix pdfs, NumericMatrix w_mat) // why null distribution is used?
{
	long n = pdfs.nrow();
	// Compute the normalizing factor under the null :
	long i, j;
	double z = 0.0;
	NumericMatrix null_distribution(n, n);  // set size 

	if (w_mat.nrow() == 1)  // same w for all 
	{
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
			{
				null_distribution(i, j) = w_mat(1, 1) * pdfs(i, 0) * pdfs(j, 1);
				z += null_distribution(i, j);
			}
	}
	else
	{
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++)
			{
				null_distribution(i, j) = w_mat(i, j) * pdfs(i, 0) * pdfs(j, 1);
				z += null_distribution(i, j);
			}
	}
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			null_distribution(i, j) /= z;  // normalize 

	List ret;
	ret["Z"] = z; // uppercase Z
	ret["distribution"] = null_distribution;
	return(ret); 
}




// double PDFToCDF2d(double** pdf_2d, double* data[2], long n, double** cdf_2d)
// [[Rcpp::export]]
NumericMatrix PDFToCDF2d_rcpp(NumericMatrix pdf_2d, NumericMatrix data)
{
	long i, j;

	long n = pdf_2d.nrow();
	IntegerVector Px(n);
	IntegerVector Py(n);

	NumericVector data0 = data(_, 0);
	NumericVector data1 = data(_, 1);

	Px = sort_indexes_rcpp(data0); //  data(_, 0));
	Py = sort_indexes_rcpp(data1); //  data(_, 1));

	NumericMatrix cdf_2d(n, n);
	NumericMatrix cdf_2d_ret(n, n);

	// cumulative sum on rows 
	for (i = 0; i < n; i++)
	{
		cdf_2d(Px[i], Py[0]) = pdf_2d(Px[i], Py[0]);
		for (j = 1; j < n; j++)
			cdf_2d(Px[i], Py[j]) = cdf_2d(Px[i], Py[j - 1]) + pdf_2d(Px[i], Py[j]);
	}
	// cumulative sum on columns 
	for (i = 1; i < n; i++)
		for (j = 0; j < n; j++)
			cdf_2d(Px[i], Py[j]) = cdf_2d(Px[i - 1], Py[j]) + cdf_2d(Px[i], Py[j]);

	// Fix ties 
	for (i = n - 1; i > 0; i--) 
		if (data(Px[i - 1], 0) == data(Px[i], 0))
			for(j = 0; j < n; j++)
				cdf_2d(Px[i - 1], j) = cdf_2d(Px[i], j);
	for (j = n - 1; j > 0; j--)
		if (data(Py[j - 1], 1) == data(Py[j], 1))
			for (i = 0; i < n; i++)
				cdf_2d(i, Py[j - 1]) = cdf_2d(i, Py[j]);


	return(cdf_2d); // don't permute back
}



// double GetQuarterExpectedProb(double Point[2], long QId, double* data[2], double** null_distribution_CDF, long n)
// [[Rcpp::export]]
double GetQuarterExpectedProb_rcpp(NumericVector Point, long QId, NumericMatrix data, NumericMatrix null_distribution_CDF)
{
	// convert below R to C++:
	long idx_x = -1, idx_y = -1, i;
	double s = 0.0;
	long n = data.nrow();

	double epsilon = 0.00000000000001; // should be much smaller than grid perturbation 
	double point_x_minus = Point[0];
	double point_y_minus = Point[1];
	if ((QId == 2) || (QId == 3)) // work with 0-3 coordinates for quadrants
		point_x_minus -= epsilon;
	if ((QId == 1) || (QId == 2))
		point_y_minus -= epsilon; 

	for (i = 0; i < n; i++)
		if (data(i, 0) <= point_x_minus)
			if ((idx_x == -1) || (data(idx_x, 0) < data(i, 0)))  // take minimal value larger than Point[0]
				idx_x = i;
	for (i = 0; i < n; i++)
		if (data(i, 1) <= point_y_minus)
			if ((idx_y == -1) || (data(idx_y, 1) < data(i, 1)))
				idx_y = i;

	double cdf_point = 0.0, cdf_x = 0.0, cdf_y = 0.0;
	if ((idx_x > -1) && (idx_y > -1))
		cdf_point =  null_distribution_CDF(idx_x, idx_y);

	long idx_x_max, idx_y_max;
	if ((QId == 0) || (QId == 1)) 
	{
		if(idx_y > -1)
		{
			idx_x_max = which_max(data(_, 0));
			cdf_y = null_distribution_CDF(idx_x_max, idx_y);
		}
	}
	if ((QId == 0) || (QId == 3)) 
	{
		if(idx_x > -1)
		{
			idx_y_max = which_max(data(_, 1));
			cdf_x = null_distribution_CDF(idx_x, idx_y_max);
		}
	}

	switch (QId) { // different quardants
	case 0: {s = 1 + cdf_point - cdf_x - cdf_y; break; } // "huji" prints "1",
	case 1: {s = cdf_y - cdf_point; break; }
	case 2: {s = cdf_point; break; }
	case 3: {s = cdf_x - cdf_point; break; }
	} // end switch 
	
	return(s);
}

// [[Rcpp::export]]
double GetQuarterExpectedProb_rcpp2(NumericVector Point, long QId, NumericMatrix data, NumericMatrix null_distribution_CDF)
{
	double s = 0.0; 
	double max_x = max(data(_,0)); 
	double max_y = max(data(_,1));
	NumericVector Point_minus = Point;
	double epsilon = 0.0000000000001;
	Point_minus[0] -= epsilon;
	Point_minus[1] -= epsilon;
	NumericVector Point_max = Point;

///	Rcout << " Quarter: " << QId << " max_x = " << max_x << " max_y = " << max_y << endl; 
	switch (QId) { // different quardants
	case 0:	{
		s = 1.0 +  ecdf2_rcpp(Point, null_distribution_CDF, data); ///Rcout << "Did 1st ";
		Point_max[0] = Point[0]; Point_max[1] = max_y; 
		s -= ecdf2_rcpp(Point_max, null_distribution_CDF, data); ///Rcout << "Did 2nd ";
		Point_max[0] = max_x; Point_max[1] = Point[1]; 
		s -= ecdf2_rcpp(Point_max, null_distribution_CDF, data);  break;	}
	case 1: {
		Point_max[0] = max_x; Point_max[1] = Point_minus[1];
		s = ecdf2_rcpp(Point_max, null_distribution_CDF, data);
		Point_max[0] = Point[0]; Point_max[1] = Point_minus[1];
		s -= ecdf2_rcpp(Point_max, null_distribution_CDF, data); break;}
	case 2: {
		s = ecdf2_rcpp(Point_minus, null_distribution_CDF, data); break;}
	case 3: {
		Point_max[0] = Point_minus[0]; Point_max[1] = max_y;
		s = ecdf2_rcpp(Point_max, null_distribution_CDF, data);
		Point_max[0] = Point_minus[0]; Point_max[1] = Point[1];
		s -= ecdf2_rcpp(Point_max, null_distribution_CDF, data);}
	} // end switch 
	return(s);
}


// [[Rcpp::export]]
NumericMatrix QuarterProbFromBootstrap_rcpp(NumericMatrix data, NumericMatrix null_distribution, NumericMatrix grid_points)
{
	long i, j;
	long n = data.nrow();
	long n_grid = grid_points.nrow();
	NumericMatrix null_distribution_CDF(n, n);
	NumericVector cur_grid_points(2);
	NumericMatrix mass_table(n_grid, 4);

///	Rcout << "Running PDF->CDF2d" << endl;
	null_distribution_CDF = PDFToCDF2d_rcpp(null_distribution, data);  // convert PDF to CDF 
///	Rcout << "Did PDF->CDF2d" << endl;

//	cout << "n: " << n << " n_grid: " << n_grid << " null-dist: " << null_distribution.nrow() << ", " << null_distribution.ncol() << endl;
	for (i = 0; i < n_grid; i++)  // loop on grid-points
	{
///		Rcout << "Copy i=" << endl; 
		cur_grid_points = grid_points(i, _); // 		cur_grid_points[1] = grid_points(i,1);
///		Rcout << "Copied i=" << endl; 
		for (j = 0; j < 4; j++)  // compute probabilities (normalize outside the function)
		{
///			Rcout << "Try2 exp (" << i << ", " << j << ")";	
			mass_table(i, j) = GetQuarterExpectedProb_rcpp2(cur_grid_points, j, data, null_distribution_CDF); // try new version 
///			Rcout << " mass2=" << mass_table(i, j); 
		}
///		Rcout << endl;
//		mass_table(i,3) = n - mass_table(i,2) - mass_table(i,1) - mass_table(i,0);  // we can't complete to one 
	}
///	Rcout << "Did loop" << endl; 
	return(mass_table);
}  // end function QuarterProbFromBootstrap




// [[Rcpp::export]]
NumericMatrix QuarterProbFromPermutations_rcpp(NumericMatrix data, NumericMatrix P, NumericMatrix grid_points)
{
	long i, j, k, a, b;
	long n = data.nrow();
	NumericMatrix mass_table(n, 4);
	for (i = 0; i < grid_points.nrow(); i++) // loop over grid points 
	{
		for (j = 0; j < n; j++)
		{
			a = data(j, 0) <= grid_points(i, 0);
			for (k = 0; k < n; k++)
			{
				b = data(k, 1) <= grid_points(i, 1);
				mass_table(i, 2*a + a*(1-b) + (1-a)*b) += P(j, k);
			}
		}
	}
	return(mass_table);
}


/*
###################################################################################################
# Compute quarter probabilities for the standard bivariate Gaussians 
# with truncation y>x. We assume grid.points satisfy y>x
#
# grid.points - where to evaluate probabilitites 
###################################################################################################*/
// [[Rcpp::export]]
NumericMatrix QuarterProbGaussianAnalytic_rcpp(NumericMatrix grid_points, string w_fun)
{
  	long n = grid_points.nrow();	
  	NumericMatrix mass_table(n, 4); 
	NumericVector x = grid_points(_,0); 
	NumericVector y = grid_points(_,1); 

	for(long i = 0; i < n; i++)
	{
		if((w_fun == "truncation") || (w_fun == "Hyperplane_truncation"))
		{
			mass_table(i,0) =  (1-normalCDF(y[i]))*(1+normalCDF(y[i])-2*normalCDF(y[i]));
			mass_table(i,1) =  pow((normalCDF(x[i])-normalCDF(y[i])), 2);
			mass_table(i,2) =  normalCDF(x[i])*(2*normalCDF(y[i])-normalCDF(x[i]));
			mass_table(i,3) =  2*normalCDF(x[i])*(1-normalCDF(y[i]));
		}
		if((w_fun == "const") || (w_fun == "naive"))
		{
			mass_table(i,0) =  (1-normalCDF(x[i]))*(1-normalCDF(y[i]));
			mass_table(i,1) =  (1-normalCDF(x[i]))*normalCDF(y[i]);
			mass_table(i,2) =  normalCDF(x[i])*normalCDF(y[i]);
			mass_table(i,3) =  normalCDF(x[i])*(1-normalCDF(y[i]));
		}
	}
  	return(mass_table);
}


/****************************************************************************************************************************/
// Here we add ALL function files from multiple files into a single cpp file (to enable easier compilation with sourcecpp).
// This should be replaced later with a pakcage. 
/****************************************************************************************************************************/

/**
##############################################################################
# Convert marginal PDF to CDF from data
# Parameters:
# data - k * n array of variables
# PDF.table - matrix with every column a vector of PDF of each variable
# Output:
# CDF.table - matrix with every column a vector of CDF of each variable
##############################################################################
**/
// [[Rcpp::export]]
NumericMatrix PDFToCDFMarginals_rcpp(NumericMatrix data, NumericMatrix PDFs)
{
	long c, i;
	long n = PDFs.nrow(); 
	NumericMatrix CDFs(PDFs.nrow(), PDFs.ncol()); //	CDF.table < -array(0L, dim(PDF.table))  # matrix(0, num.samples, num.variables)
	IntegerVector Px(n);

	for (c = 0; c < PDFs.ncol(); c++) // loop on variables 
	{
//		Rcout << "PDF -> CDF COL=" << c << endl; 
		Px = sort_indexes_rcpp(data(_, c)); // get indexes permute to order x_i, y_i

//		cout << "sort perm outside:" << endl;
//		for (i = 0; i < Px.length(); i++)
//			Rcout << Px[i] << ", ";
//		Rcout << endl << "Sorted Indexes 0: " << Px[0] << ", n=" << n << endl; 
		CDFs(Px[0], c) = PDFs(Px[0], c);
		for (i = 1; i < n; i++)
		{
//			Rcout << "Copy Index:" << i << " P[]=" << Px[i-1] << ", " << Px[i] << endl;
			CDFs(Px[i], c) = CDFs(Px[i - 1], c) + PDFs(Px[i], c); //  cumsum(PDFs(Px, c)); // need to change 
		}
//		Rcout << "Computed CDFs" << endl;

		for (i = n - 1; i > 0; i--) // fix ties 
			if (data(Px[i-1], c) == data(Px[i], c))
				CDFs(Px[i-1], c) = CDFs(Px[i], c);
//		Rcout << "Copy Ties" << endl;

	}
	return(CDFs);
}


// [[Rcpp::export]]
NumericMatrix CDFToPDFMarginals_rcpp(NumericMatrix CDFs)
{
	long c, i;
	long n = CDFs.nrow();
	NumericMatrix PDFs(CDFs.nrow(), CDFs.ncol());
	IntegerVector Px(n);
	NumericVector CDF_sorted(n);

	for (c = 0; c < CDFs.ncol(); c++)
	{
		Px = sort_indexes_rcpp(CDFs(_, c));
		CDF_sorted = CDFs(_, c);
		CDF_sorted = CDF_sorted.sort(); //  sort(CDFs(_, c));
		
		PDFs(Px[0], c) = CDF_sorted[0]; // cumsum
		for (i = 1; i < n; i++)
			PDFs(Px[i], c) = CDF_sorted[i] - CDF_sorted[i - 1];
	}
	return(PDFs);
}


// [[Rcpp::export]]
List EstimateMarginals_rcpp(NumericMatrix data, string w_fun)  
{
	List ret;
	
	long i, j;
	long n = data.nrow();
	NumericVector w_inv(n); //	double* w_inv = new double[n];
	double w_inv_sum = 0.0;
	NumericMatrix PDFs(2*n, 2);
	NumericMatrix CDFs(2*n, 2);

//	NumericMatrix CDFs_alt(2 * n, 2);

	long naive_flag = FALSE;
//	string pos_w[4] = {"sum", "sum_coordinates", "exponent_minus_sum_abs", "const"}; // all positive w -> should replace by a function checking for positivity 
	string naive_w[2] = { "naive", "no_bias" };
	long pos_flag = is_pos_w_rcpp(w_fun, data, 0); // take mat_flag = 0

//	for (i = 0; i < 4; i++)/
//		if (w_fun == pos_w[i])
//			pos_flag = TRUE;
	for (i = 0; i < 2; i++)
		if (w_fun == naive_w[i])
			naive_flag = TRUE;

//	cout << "POS FLAG: " << pos_flag << endl; 
	if (pos_flag && (!naive_flag)) // w_fun % in % c('sum', 'sum_coordinates', 'exponent_minus_sum_abs')) // for w(x, y) > 0 cases
	{ // case 1: strictly positive W, use ML estimator
		for (i = 0; i < n; i++)
		{
			w_inv[i] = 1.0 / w_fun_eval_rcpp(data(i, 0), data(i, 1), w_fun);
			w_inv_sum += w_inv[i];
		}
		for (i = 0; i < n; i++)   // normalize
			PDFs(i, 0) = PDFs(i, 1) = w_inv[i] / w_inv_sum;
		PDFs = PDFs(Range(0, n-1), _);
		CDFs = PDFToCDFMarginals_rcpp(data, PDFs);  // why null ?
		ret["xy"] = data;
	}
	else { // not positive 
		// skip survival 
		if (naive_flag) // no bias(assume W(x, y) = 1)
		{
			CDFs = CDFs(Range(0, n - 1), _);
			CDFs(_, 0) = ecdf_rcpp(data(_, 0));
			CDFs(_, 1) = ecdf_rcpp(data(_, 1));
			ret["xy"] = data;
		}
		else { // use W , symmetric , exchangable 
				// general biased sampling function(w.fun can be a function not a string) with exchangable distributions
			// case 2: left truncation, use the estimator of Proposition 1 in the paper for exchangable distributions
					// Augment data to include both xand y values for each axis(due to symmetry)
			double augment_data = TRUE;   // new: add xy values
			NumericMatrix new_data(2*n, 2); // largest 
			NumericMatrix new_data_sorted(2*n, 2);
			if(augment_data) // duplicate x and y values
			{
				for (j = 0; j < n; j++)
					for (i = 0; i < 2; i++)
					{
						new_data(j, i) = data(j, 0);  // here 0, 1 are the same 
						new_data(j+n, i) = data(j, 1);
					}
			}
			else // just copy data 
			{
				for (i = 0; i < 2; i++)
					for (j = 0; j < n; j++)
						new_data(j, i) = data(j, i);
			}
			// need here permutations !! 
			NumericVector Px(2 * n);
			NumericVector Py(2 * n);
			Px = sort_indexes_rcpp(new_data(_, 0));
			Py = sort_indexes_rcpp(new_data(_, 1));

//			Rcout << "Generated Px, Py" << endl; 
//			for (i = 0; i < 5; i++) // copy sorted 
//				Rcout << Px[i] << " " << Py[i] << endl; 

			for (i = 0; i < 2*n; i++) // copy sorted 
			{
				new_data_sorted(i, 0) = new_data(Px[i], 0);
				new_data_sorted(i, 1) = new_data(Py[i], 1);
			}
//			Rcout << "Generated new sorted data:" << endl;
//			for (i = 0; i < 5; i++) // copy sorted 
//				Rcout << new_data_sorted(i,0) << " " << new_data_sorted(i, 1) << endl;
			NumericVector F0 = ecdf_rcpp(new_data_sorted(_, 0)); // can be used instead of binary search 
			NumericVector F1 = ecdf_rcpp(new_data_sorted(_, 1));
			// alternative: take average of F0 and F1
			CDFs(_, 0) = (F0 + F1) / 2;
			CDFs(_, 1) = CDFs(_, 0); //  (F0 + F1) / 2;

			ret["xy"] = new_data;
		} // end if naive w

//		Rcout << " Now CDF-PDF Marginals" << endl; 

		PDFs = CDFToPDFMarginals_rcpp(CDFs); // convert (this part is fast)
//		Rcout << " Finished CDF-PDF Marginals" << endl;
	} // end if positive w

	ret["PDFs"] = PDFs;
	ret["CDFs"] = CDFs;	

//	Rcout << " Returning Marginals" << endl;
	return(ret);

}  // end function 


/**
#################################################################
# sample permutations, using MCMC, over the set of valid permutations,
# with respect to the distribution appears in Eq 8
# Parameters:
# W - matrix with weights
# B - number of permutations to draw
# N - sample size(can be read from data or W ? )
# 
# Output:
# PermutationsTable - A matrix representing the sampled permutations
# P - An n * n matrix with P(i, j) = Pr(pi(i) = j) over the sampled permutations
#########################################################################################
**/

// [[Rcpp::export]]
List PermutationsMCMC_rcpp(NumericMatrix w_mat, List prms) // burn.in = NA, Cycle = NA)  # New: allow non - default burn - in
{
	long n = w_mat.nrow();
	NumericMatrix P(n, n);  // New!matrix with P[i] = j estimate
	long i, j, temp;
	double ratio;

	// Set mcmc default sampling parameters
	long B = 1000;
	if (prms.containsElementNamed("B")) //   ('B' % in % names(prms)))
		B = prms["B"];
	long burn_in = 0; //  2 * n;
	if (prms.containsElementNamed("burn_in")) //  ('burn.in' % in % names(prms)))
		burn_in = prms["burn_in"];
	long Cycle = n; 
	if (prms.containsElementNamed("Cycle")) // (!('Cycle' % in % names(prms)))
		Cycle = prms["Cycle"];

	long Idx = 0, ctr = 1;

//	Rcout << "burn.in=" << burn_in << " Cycle=" << Cycle << endl; 

//	Rcout << "Perms dimensions: n=" << n << " B = " << B << endl;
	NumericMatrix Permutations(n, long(B));
	IntegerVector Perm = seq(0, n);  // 1 to N-1 
	NumericVector switchIdx(2);
	while (Idx < B)
	{
//		Rcout << "Draw Perm=" << Idx << endl; 
	// A Metropolis Hastings algorithm with target stationary distribution \pi
	// Choose the two indices to be switched
		switchIdx[0] = rand() % n; 
		switchIdx[1] = switchIdx[0];
		while (switchIdx[1] == switchIdx[0])
			switchIdx[1] = rand() % n; // sample without replacement form 0 to n-1
		i = switchIdx[0];
		j = switchIdx[1];
		ratio = w_mat(i, Perm[j]) * w_mat(j, Perm[i]) / (w_mat(i, Perm[i]) * w_mat(j, Perm[j]));

//		ratio = w.mat[i, Perm[j]] * w.mat[j, Perm[i]] / (w.mat[i, Perm[i]] * w.mat[j, Perm[j]]) // R code !! 


		if(	double(rand()) / RAND_MAX < fmin(1.0, ratio) ) // we accept the transition with probability min(1, ratio)
		{
			temp = Perm[i];  // SWAP
			Perm[i] = Perm[j];
			Perm[j] = temp;
			if (ctr == burn_in || ((ctr % Cycle == 0) && (ctr > burn_in)))
			{
				for(i=0; i<n; i++)
					Permutations(i,Idx) = Perm[i] + 1;  // add 1 to move from C zero-based indexing to R-one based indexing. A problem if we use it INSIDE C!!!! 
				Idx++; 
//				if (Idx%100 == 0)
//					Rcout << "Sample Perm=" << Idx << endl;
			}
			for (i = 0; i < n; i++)
				P(i, Perm[i])++; // Update table counts P
			ctr++;   // update counter only after swap
		}
	}  // end while
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			P(i, j) = P(i, j) / (ctr - 1); //  normalize

	List ret;
	ret["Permutations"] = Permutations;
	ret["P"] = P;
	return(ret); // return list with also P
}



// Sample a random permutation from the uniform distribution over S_n 
// [[Rcpp::export]]
NumericVector rand_perm(long n)
{
	NumericVector perm(n); 
	long i, temp, r;

	for (i = 0; i < n; i++)
		perm[i] = i; // start with the identity 
	for (i = 0; i < n; i++)
	{
		r = rand() % (n - i) + i;
		temp = perm[i]; // swap 
		perm[i] = perm[r];
		perm[r] = temp; 
	}
	return(perm);
}

/**
#############################################################
# Calculate p - value using importance sampling
# (If product of weights is very small/large, we can
	#  multiply the weight function by a constant so the weights
	#  are in some sense centered at 1)
#############################################################
**/
// [[Rcpp::export]]
List IS_permute_rcpp(NumericMatrix data, NumericMatrix grid_points, double B, string w_fun, NumericMatrix expectations_table) 
{
	long  n = data.nrow();
	double Tobs; // = ComputeStatistic_w_rcpp(data, data, w_fun); 
	double reject = 0.0; //  , sum_p = 0;
	long i, j;
	NumericVector pw(B); 
	NumericVector Tb(B); 
	IntegerVector perm(n);
	NumericMatrix permuted_data(n, 2);

	if(grid_points.ncol() == 1) // no input grid
		grid_points = data;

  	long inverse_weight = (expectations_table.ncol() == 1);   // | isempty(expectations.table) # default is using inverse weighting 
//	Rcout << "Start IS Permute Inverse Weight: " << inverse_weight << " Dim(Exp-Table)=" << expectations_table.nrow() << ", " << expectations_table.ncol() << endl; 
  	if(inverse_weight)
    	Tobs = ComputeStatistic_w_rcpp(data, grid_points, w_fun); // weights. no unique in grid-points 
	else
    	Tobs = ComputeStatistic_rcpp(data, grid_points, expectations_table); // no weights

	for (i = 0; i < B; i++) 
	{
		perm = rand_perm(n);  // get a random permutation from the uniform disitribution
		permuted_data(_, 0) = data(_, 0);
		for (j = 0; j < n; j++)
			permuted_data(j, 1) = data(perm[j], 1); // permute data 
		if(inverse_weight)
			Tb[i] = ComputeStatistic_w_rcpp(permuted_data, grid_points, w_fun); // grid depends on permuted data. Compute weighted statistic! 
		else 
			Tb[i] = ComputeStatistic_rcpp(permuted_data, grid_points, expectations_table); // grid depends on permuted data. Compute weighted statistic! 
		pw[i] = 0.0;
		for(j=0; j<n; j++)
			pw[i] += log(w_fun_eval_rcpp(permuted_data(j,0), permuted_data(j,1), w_fun));  // take log to avoid underflow 
		
	}

	double pw_max = max(pw);
	for (i = 0; i < B; i++)
	{
		pw[i] = exp(pw[i] - pw_max);
//		Rcout << "i = " << i << " pw[i]=" << pw[i] << endl; 
		reject += (Tb[i] >= Tobs) * pw[i];
//		sum_p += pw[i];
	}

	List ret; 
	ret["Pvalue"] = reject / sum(pw);
	ret["TrueT"] = Tobs; 
	ret["statistics.under.null"] = Tb;
//	ret["statistics.under.null"] = Tb; // get null statistics. But they're not weighted equally!
	return(ret);
}


/**
##############################################################################
# Iterative Algorithm for marginal estimation from samples of F_{ XY }^ {(w)}
# Parameters:
# data - 2 * n array of(X, Y)
# CDF.table - vector of CDF of X and Y
# Output:
# 
##############################################################################
**/
// [[Rcpp::export]]
NumericMatrix iterative_marginal_estimation_rcpp(NumericMatrix data, string w_fun)
{
	long n = data.nrow();
	long i; 
	double epsilon = 0.0000001;
	long iters = 1000;
	NumericVector f_x(n);
	NumericVector f_y(n);
	NumericVector f_x_prev(n);
	NumericVector f_y_prev(n);
	NumericVector temp_f_x(n);
	NumericVector temp_f_y(n);

	double change;
	NumericMatrix w_mat = w_fun_to_mat_rcpp(data, w_fun);

	// arma::mat w_mat_arma = as<arma::mat>(w_mat);

	long t;
	for (t = 0 ; t < iters; t++)
	{
		
		f_x_prev = f_x;
		f_y_prev = f_y;
		
		temp_f_x = my_mv_mult(w_mat, f_y);
		for (i = 0; i < n; i++)
			f_x[i] = 1.0 / temp_f_x[i];
		temp_f_y = my_mv_mult(transpose(w_mat), f_x); // take transpose 
		for (i = 0; i < n; i++)
			f_y[i] = 1.0 / temp_f_y[i];

			
//			f_x = 1.0 / (w_mat * f_y);
//		f_y = 1.0 / (w_mat.t() * f_x); // multiply by transpose 

		change = 0.0; 
		for (i = 0; i < n; i++)
			change += (abs(f_x[i] - f_x_prev[i]) + abs(f_y[i] - f_y_prev[i]));
		if (change < epsilon)
			break;
	}
	NumericMatrix f_xy(n, 2);
	f_xy(_, 0) = f_x;
	f_xy(_, 1) = f_y;
	return(f_xy);  // PDF.table
}


/**
############################################################################################
# Draw a bootstrap sample, given the estimated null distribution Fx, Fy, W
# Use rejection sampling.No need for computing n* n table
# (Problem: what if prob.(rejection) close to 1?)
# Parameters:
# data - n * 2 array with(X, Y) samples
# cdfs - fx and fy - cumulatives (not pdfs!)
# w.fun - Biased sampling function w
# prms - for w max
# n - allow a different sample size
# Output: 
# an n*2 matrix of bootstrap sample 
############################################################################################**/
// [[Rcpp::export]]
List Bootstrap_rcpp(NumericMatrix data, NumericMatrix cdfs, NumericMatrix w_mat,  List prms, long n=-1)  // n is optional . Currently very slow!
{
	if (n == -1) // is.null(n))
		n = data.nrow();
	IntegerMatrix boot_indices(n, 2);
	NumericMatrix boot_sample(n, 2);


//	double x = 0.0, y = 0.0; //  , r; //  keep;
	long k = 0, ctr=0; 
	long i=0, j=0;
	double w_max = prms["w.max"];

//	Rcout << "Start Bootstrap, w_max=" << w_max << endl; 
//	long m;
	long a, b, t, mid=(n-1)/2;
	double r;
	while (k < n)
	{	
		ctr++;	
		for(t=0; t<2; t++)
		{
			r = double(rand()) / RAND_MAX;
			a = 0;  b = n - 1;  // do binary search 
			while (a < b) 
			{
				mid = (a + b) / 2;
				if (cdfs(mid,t) < r) 
					a = mid+1;
			    else 
					b = mid;
			}
			if(t == 0)
				i = mid;
			else
				j = mid;
		}
//		x = data(i, 0); y = data(j, 1); //		rand() data(d0(gen), 0); //  rand() % (n - k); // sample with replacement 		 //data(d1(gen), 1); //  rand() % (n - k);
//		if ((double(rand()) / RAND_MAX) < (w_fun_eval_rcpp(x, y, w_fun) / w_max)) // compare to probability (rejection - can be costly when w_max is large!!)
//		Rcout << "Try k=" << k << " i=" << i << " j=" << j << " w(x,y)=" << w_mat(i, j) << endl; 
		if ((double(rand()) / RAND_MAX) < (w_mat(i, j) / w_max)) // compare to probability (rejection - can be costly when w_max is large!!)
		{
			boot_indices(k, 0) = i; boot_indices(k, 1) = j;
			boot_sample(k, 0) = data(i, 0); boot_sample(k++, 1) = data(j, 1);
		}
//		if((i < 0) || (i >=n) || (j < 0) || (j>=n))
//			Rcout << "ERROR! x,y: " << x << ", " << y << " k=" << k << endl; 
		ctr++;
	}
	//	Rcout << "Generated " << ctr << " pairs to get eventually " << n << " samples" << endl; 
	List ret;
	ret["sample"] = boot_sample; 
	ret["indices"] = boot_indices;

//	Rcout << "End Bootstrap Rcpp, w_max=" << w_max <<  " waste-factor=" << double(ctr)/double(n) << endl; 
//	Rcout << "w_max=" << w_max << endl; 
//	for (i = 0; i < 5; i++)
//		for (j = 0; j < 2; j++)
//			Rcout << "BootInd(i,j)=" << i << ", " << j << " Fill Ind=" << boot_indices(i, j) << " Val=" << boot_sample(i, j) << endl;
	return(ret);
}


// A special function for copying bootstrap data 
List BootstrapOrganize_rcpp(List data_marginals, List bootstrap, string w_fun, List prms)
{
	long i, j;
	long n = prms["sample.size"];
	long marginals_n = n; 
	if((w_fun == "truncation") || (w_fun == "Hyperplane_Truncation"))
		marginals_n *= 2; 

	NumericMatrix bootstrap_indices = as<NumericMatrix>(bootstrap["indices"]); 
	List marginals_bootstrap = EstimateMarginals_rcpp(as<NumericMatrix>(bootstrap["sample"]), w_fun);

	NumericMatrix marginals_bootstrap_PDFs = marginals_bootstrap["PDFs"];
  	List marginals_bootstrap_organized; 
	marginals_bootstrap_organized["xy"] = as<NumericMatrix>(data_marginals["xy"]);
	NumericMatrix marginals_bootstrap_organized_PDFs(marginals_n, 2);
	if((w_fun == "truncation") || (w_fun == "Hyperplane_Truncation")) // here data was duplicated 
	{
		for(i=0; i<n; i++)
			for(j=0; j<2; j++)
			{
//				Rcout << "i, j, inds: " << i << ", "  << j << ", " << bootstrap_indices(i, j) << endl; 
				marginals_bootstrap_organized_PDFs(bootstrap_indices(i, j), 0) += marginals_bootstrap_PDFs(i+j*n, j); 
				marginals_bootstrap_organized_PDFs(bootstrap_indices(i, j), 1) += marginals_bootstrap_PDFs(i+j*n, j); 
			}
	} else
	{
		for(i=0; i<n; i++)
			for(j=0; j<2; j++)
				marginals_bootstrap_organized_PDFs(bootstrap_indices(i, j), j) += marginals_bootstrap_PDFs(i, j); 
	}
	/**/
//	NOT NEEDED !!! NumericMatrix marginals_bootstrap_organized_CDFs = marginals_bootstrap_organized_PDFs; //// TEMP! FOR TIMING !!! PDFToCDFMarginals_rcpp(data_marginals["xy"], marginals_bootstrap_organized_PDFs); // replaced data by marginals["xy"]

	marginals_bootstrap_organized["PDFs"] = marginals_bootstrap_organized_PDFs;
//	marginals_bootstrap_organized["CDFs"] = marginals_bootstrap_organized_CDFs;

	return(marginals_bootstrap_organized);
}


/**########################################################################
# 
# All steps of TIBS test :
# 1. estimate marginals
# 2. compute null distribution
# 3. compute expectation table
# 4. compute statistic
# 
########################################################################**/
// [[Rcpp::export]]
List TIBS_steps_rcpp(NumericMatrix data, string w_fun, NumericMatrix w_mat, NumericMatrix grid_points, NumericMatrix expectations_table, List prms)
{
	List marginals;
	List null_distribution;
	string use_w;
	long n = data.nrow(); 
//	Rcout << "Start TIBS STEPS" << endl; 
	long naive_expectation = 0;
	if (prms.containsElementNamed("naive.expectation"))
		naive_expectation = prms["naive.expectation"];

	if (naive_expectation) // here we ignore W(using statistic for unbiased sampling)
	{
		use_w = "naive";
		w_mat(1, 1) = 1;
	}
	else
		use_w = w_fun;
		
	if(expectations_table.nrow()==1) // insert empty table // 	if (missing(expectations_table) | isempty(expectations_table)) // modify to c++ code!
	{
//		Rcout << "USE BOOTSTRAP EXPECTED!" << endl; 
		marginals = EstimateMarginals_rcpp(data, use_w);
//		Rcout << "Finished Estimating BOOTSTRAP EXPECTED!" << endl; 
		if ((!naive_expectation) & (w_mat.nrow() == 1))
			w_mat = w_fun_to_mat_rcpp(marginals["xy"], w_fun);  // compute W again for augmented data
//		Rcout << "Computed w_mat!" << endl;
		null_distribution = GetNullDistribution_rcpp(marginals["PDFs"], w_mat);
//		Rcout << "Computed Null Distribution!" << endl; 
		expectations_table = double(n) * QuarterProbFromBootstrap_rcpp(marginals["xy"], null_distribution["distribution"], grid_points);
/*		Rcout << "Computed BOOTSTRAP QuarterProb EXPECTED!" << endl;
		long i, j; 
		for(i=0; i<5; i++)
		{
			for(j=0; j<4; j++)
				Rcout << " " << expectations_table(i,j); 
			Rcout << endl;
		}
		*/
	}
	double T = ComputeStatistic_rcpp(data, grid_points, expectations_table);  // keep same grid points for bootstrap sample ?

	List ret; ret["Statistic"] = T; ret["expectations_table"] = expectations_table; 
	ret["marginals"] = marginals; ret["w_mat"] = w_mat; ret["null_distirbution"] = null_distribution; 
	return(ret);
}



/**
########################################################################
# Perform Test for Independence under general Biased Sampling(TIBS)
# 
# Parameters:
# data - n * 2 array of(x, y) samples(NOT LIST!)
# w.fun - biased sampling function W
# test_type - test to perform
# prms - additional parameters(needed for bootstrap) including B - number of bootstrap / permutation samples to perform
# 
# Output:
# TrueT - test statistic for the data
# statistics_under_null - vector of statistics under the null
########################################################################
**/
// [[Rcpp::export]]
List TIBS_rcpp(NumericMatrix data, string w_fun, string test_type, List prms)
{
	double epsilon = 0.00000000000001;
	long ctr;
	double TrueT, NullT;
	List output; 
//	Rcout << "Start TIBS " << endl; 

	// Set defaults
	long fast_bootstrap = FALSE;
	if (prms.containsElementNamed("fast.bootstrap"))
		fast_bootstrap = prms["fast.bootstrap"];
//	if (!prms.containsElementNamed("minp_eps"))
//		prms["minp_eps"] = NULL; // permDep params. Irrelevant here 
	long PL_expectation = FALSE;
	if (prms.containsElementNamed("PL.expectation"))
		PL_expectation = prms["PL.expectation"];
	long naive_expectation = 0;
	if (prms.containsElementNamed("naive.expectation"))
		naive_expectation = prms["naive.expectation"];
	long new_bootstrap = TRUE; 
	if (prms.containsElementNamed("new.bootstrap"))
		new_bootstrap = prms["new.bootstrap"];
	long perturb_grid = TRUE; // add a small perturbation to avoid ties (default: TRUE)
	if (prms.containsElementNamed("perturb.grid"))
		perturb_grid = prms["perturb.grid"];


//	if (!prms.containsElementNamed("delta"))
//		prms["delta"] = NA; minP2 params. Irrelevant here

	long B = prms["B"];
	long n = data.nrow();
	long i, j; 

//	Rcout << "Start TIBS n= " << n << endl; 
 	NumericMatrix new_data(n, 2); // copy data 
  	for (i = 0; i < n; i++) // long(grid_points_arma.n_rows); i++)
    	for (j = 0; j < 2; j++)
        	new_data(i, j) = data(i, j);
	
//	Rcout << " Read Input TIBS_RCPP. TEST-TYPE: " << test_type << endl; 
	// Create a grid of points, based on the data :
	NumericMatrix grid_points = new_data;
	if(perturb_grid)
	{
		NumericMatrix perturb(n, 2);
		perturb(_, 0) = epsilon * rnorm(n);
		perturb(_, 1) = epsilon * rnorm(n);
		grid_points += perturb; 
	}
/*	arma::mat grid_points_arma = unique_rows(as<arma::mat>(data));  // set unique for ties ? for discrete data
	NumericMatrix grid_points(n, 2);
	for (i = 0; i < n; i++) // long(grid_points_arma.n_rows); i++)
		for (j = 0; j < 2; j++)
			grid_points(i, j) = grid_points_arma(i, j); **/
//	= as<NumericMatrix>(wrap(grid_points_arma));  // set unique for ties ? for discrete data
	List null_distribution;


//	Rcout << "Set TIBS grid points " << endl; 
	// no switch (test_type) for strings in cpp
	if(test_type == "bootstrap_inverse_weighting") 
	{
		List marginals = EstimateMarginals_rcpp(data, w_fun);

//		for (i = 0; i < n; i++)
	//		cout << "i: " << i << "PDF: " << as<NumericVector>(marginals["PDFs"])(i, 0) << ", " << as<NumericVector>(marginals["PDFs"])(i, 1) << 
		//	" CDF: " << as<NumericVector>(marginals["CDFs"])(i, 0) << ", " << as<NumericVector>(marginals["CDFs"])(i, 1) << endl;
		NumericMatrix w_mat = w_fun_to_mat_rcpp(marginals["xy"], w_fun);
		null_distribution = GetNullDistribution_rcpp(marginals["PDFs"], w_mat);
		TrueT = ComputeStatistic_w_rcpp(data, grid_points, w_fun); //  $Statistic
		NumericVector statistics_under_null(B);
		List bootstrap_sample; //		NumericMatrix bootstrap_sample(n, 2);
		for (ctr = 0; ctr < B; ctr++)
		{
//			if (ctr % 100 == 0)
//				Rcout << "Run Boots=" << ctr << endl;
			bootstrap_sample = Bootstrap_rcpp(marginals["xy"], marginals["CDFs"], w_mat,  prms, n); // draw new sample.Problem: which pdf and data ?
	//		NumericMatrix w_mat_bootstrap = w_fun_to_mat_rcpp(bootstrap_sample["sample"], w_fun);
			NullT = ComputeStatistic_w_rcpp(bootstrap_sample["sample"], grid_points, w_fun); // should sample grid points again! 
			statistics_under_null[ctr] = NullT; //  ["Statistic"] ;
		}
		output["TrueT"] = TrueT;
		output["statistics.under.null"] = statistics_under_null;
	}

	if(test_type == "bootstrap") 
	{
		NumericMatrix w_mat(1, 1);
		NumericMatrix expectations_table(1, 1);
//		Rcout << "Start TIBS RCPP Bootstrap Data Before" << endl;
//    for(i=0; i<5; i++)
//    Rcout << data(i,0) << " " << data(i,1) << endl; 
  
		List TrueTList = TIBS_steps_rcpp(data, w_fun, w_mat, grid_points, expectations_table, prms); // new: replace multiple steps by one function
		TrueT = TrueTList["Statistic"];
//		NumericMatrix true_expectations_table = TrueTList["expectations_table"];

	    // sort data: :
    	NumericVector data_temp = data(_, 0);
    	data(_, 0) = data_temp.sort(); //  marginals["CDFs"];
    	data_temp = data(_, 1);
    	data(_, 1) = data_temp.sort(); //  marginals["CDFs"];
    
    
//		Rcout << "Computed TrueT TIBS RCPP Bootstrap" << endl;
    // Run tibs again
		TrueTList = TIBS_steps_rcpp(data, w_fun, w_mat, grid_points, expectations_table, prms); // new: replace multiple steps by one function
		List marginals = TrueTList["marginals"]; // marginals for true data 
		List bootstrap_sample; //  (n, 2);

		// 2. Compute statistic for bootstrap sample :
		NumericVector statistics_under_null(B);
//		List null_distribution_bootstrap = null_distribution;
//		NumericMatrix w_mat_bootstrap(1,1);
		IntegerMatrix bootstrap_indices(n, 2);
		List null_distribution_bootstrap_new;
		NumericMatrix expectations_table_new(n, 4);
		List marginals_bootstrap;

		NumericMatrix CDFs_sorted = as<NumericMatrix>(marginals["CDFs"]); // get sorted CDFs 
		NumericVector CDF_temp = CDFs_sorted(_, 0);
		CDFs_sorted(_, 0) = CDF_temp.sort(); //  marginals["CDFs"];
		CDF_temp = CDFs_sorted(_, 1);
		CDFs_sorted(_, 1) = CDF_temp.sort(); //  marginals["CDFs"];
		NumericMatrix xy_sorted = as<NumericMatrix>(marginals["xy"]); // get sorted CDFs 
		NumericVector xy_temp = xy_sorted(_, 0);
		xy_sorted(_, 0) = xy_temp.sort(); //  marginals["CDFs"];
		xy_temp = xy_sorted(_, 1);
		xy_sorted(_, 1) = xy_temp.sort(); //  marginals["CDFs"];

///		Rcout << "Dims: data: " << data.nrow() << " CDF " << CDFs_sorted.nrow() << " xy " << xy_sorted.nrow() << endl; 

		List marginals_bootstrap_new;
		w_mat = w_fun_to_mat_rcpp(xy_sorted, w_fun); // compute matrix once 
		prms["w.max"] = set_w_max_rcpp_sample(data, w_fun); // set w.max for bootstrap sampling 

/**		Rcout << " TIBS RCPP Data BEFORE BOOTSTRAP" << endl;
		for(i=0; i<5; i++)
		  Rcout << data(i,0) << " " << data(i,1) << endl; 


		Rcout << " TIBS RCPP w_mat BEFORE BOOTSTRAP, w_fun=" << w_fun << endl;
		for(i=0; i<5; i++)
		{
			for(j=0; j<5; j++)
				Rcout << w_mat(i,j) << " "; 
			Rcout << endl; 
		}
		Rcout << "Run Bootstrap, NEW=" << new_bootstrap << endl; **/
		for (ctr = 0; ctr < B; ctr++) // heavy loop : run on bootstrap
			{
///				Rcout << "Run Bootstrap B = " << ctr << endl;

				// statistics_under_null[ctr] = TIBS_steps_rcpp(bootstrap_sample, w_fun, w_mat, grid_points, expectations_table, prms); // replace multiple steps by one function
////				if(ctr==0)
				bootstrap_sample = Bootstrap_rcpp(xy_sorted, CDFs_sorted /*marginals["CDFs"]*/, w_mat , prms, n); // draw new sample. Problem: which pdf and data ?
				if (!fast_bootstrap) // re - estimate marginals for null expectation for each bootstrap sample
				{
				  if (!new_bootstrap)
					{
				    List NullT = TIBS_steps_rcpp(bootstrap_sample["sample"], w_fun, NumericMatrix(1, 1), grid_points, NumericMatrix(1, 1), prms);
				    List NullT_marginals = NullT["marginals"];
						statistics_under_null[ctr] = NullT["Statistic"];
					} else
					{
///						Rcout << "Organize Marginals B = " << ctr << endl;
						marginals_bootstrap_new = BootstrapOrganize_rcpp(marginals, bootstrap_sample, w_fun, prms);
///						Rcout << "Get Null Distribution B = " << ctr << " xy : PDFs : CDFs " << endl;

						null_distribution_bootstrap_new = GetNullDistribution_rcpp(marginals_bootstrap_new["PDFs"], TrueTList["w_mat"]); // keep w_mat of ORIGINAL DATA!
///						Rcout << "Compute Expected B = " << ctr << " xy : PDFs : CDFs " << endl;
						expectations_table_new = double(n) * QuarterProbFromBootstrap_rcpp(
							marginals_bootstrap_new["xy"], null_distribution_bootstrap_new["distribution"], grid_points);
///						Rcout << "Compute Statistic B = " << ctr << " xy : PDFs : CDFs " << endl;
						statistics_under_null[ctr] = ComputeStatistic_rcpp(bootstrap_sample["sample"], grid_points, expectations_table_new); // TEMP FOR TIMING !!! _new); // NEW!Compute null statistic without recomputing the entire matrix 
						
/**						Rcout << "Expected old, new:" << endl;
						NumericMatrix NullT_expectations_table = as<NumericMatrix>(NullT["expectations_table"]);
						for(i=0; i<5; i++)
						{
						  Rcout << "Old: ";
						  for(j=0; j<4; j++)
						    Rcout << NullT_expectations_table(i, j) << " "; 
						  Rcout << endl << "New: "; 
						  for(j=0; j<4; j++)
						    Rcout << expectations_table_new(i, j) << " "; 
						  Rcout << endl; 
						}
						double z0 = max(abs(expectations_table_new - as<NumericMatrix>(NullT["expectations_table"])));
						if(z0>0.00001)
						  Rcout << "Should be zero expected table:" << z0 << endl; 
						double z1 = statistics_under_null[ctr] - as<double>(NullT["Statistic"]);
						if(abs(z1>0.00001))
						  Rcout << "Should be zero statistic:" << statistics_under_null[ctr] << ", " 
              << as<double>(NullT["Statistic"]) << endl;
**/
					}
				} else // if fast bootstrap
		//		Rcout << "Compute Bootstrap Statistic Under Null " << ctr << endl;
					statistics_under_null[ctr] = ComputeStatistic_rcpp(bootstrap_sample["sample"], grid_points, expectations_table);
			} // counter for bootstrap iterations
		
//		Rcout << " TIBS RCPP Data AFTER ALL BOOTSTRAPS" << endl;
//		for(i=0; i<5; i++)
//		  Rcout << data(i,0) << " " << data(i,1) << endl; 
		

		output["TrueT"] = TrueT;
		output["statistics.under.null"] = statistics_under_null;
	}
	
	if(test_type == "permutations")
	{
//		Rcout << "Compute w_mat Perms" << endl; 
		NumericMatrix w_mat = w_fun_to_mat_rcpp(data, w_fun);
		List PermutationsList = PermutationsMCMC_rcpp(w_mat, prms);
		NumericMatrix expectations_table(1,1);
					
		NumericMatrix P = PermutationsList["P"];
		NumericMatrix Permutations = PermutationsList["Permutations"];
		NumericMatrix permuted_data(n, 2);

		if(!(PL_expectation || naive_expectation))
			expectations_table = QuarterProbFromPermutations_rcpp(data, P, grid_points);
		List TrueTList = TIBS_steps_rcpp(data, w_fun, w_mat, grid_points, expectations_table, prms); // new: replace multiple steps by one function
		TrueT = TrueTList["Statistic"];
		permuted_data(_, 0) = data(_, 0);
		for (i = 0; i < n; i++)
			permuted_data(i, 1) = data(Permutations(i, 0)-1, 1); // save one example

/**															   //		Rcout << "Finished Permuted Data " << endl; 
		if (naive_expectation) // here we ignore W(using statistic for unbiased sampling)
		{
			List marginals = EstimateMarginals_rcpp(data, "naive");
			NumericMatrix w_mat_scalar(1, 1); w_mat_scalar(1, 1) = 1.0;
			null_distribution = GetNullDistribution_rcpp(marginals["PDFs"], w_mat_scalar);
			expectations_table = QuarterProbFromBootstrap_rcpp(marginals["xy"], null_distribution["distribution"], grid_points);
		}
		else
		{
//			Rcout << "COmpute Expectations Table" << endl; 
			if (PL_expectation)  // get expectations from the bootstrap estimator
			{
				List marginals = EstimateMarginals_rcpp(data, w_fun);
				null_distribution = GetNullDistribution_rcpp(marginals["PDFs"], w_mat);
				expectations_table = QuarterProbFromBootstrap_rcpp(marginals["xy"], null_distribution["distribution"], grid_points);
				w_mat = w_fun_to_mat_rcpp(marginals["xy"], w_fun);
			}
			else
			{
				expectations_table = QuarterProbFromPermutations_rcpp(data, P, grid_points);  // Permutations
			}
		}
//		Rcout << "Compute Statistic" << endl;
		TrueT = ComputeStatistic_rcpp(data, grid_points, expectations_table);
**/
		// Compute the statistics value for each permutation:
		NumericVector statistics_under_null(B);
		permuted_data(_, 0) = data(_, 0);
		for (ctr = 0; ctr < B; ctr++)
		{
			for (i = 0; i < n; i++)
				permuted_data(i, 1) = data(Permutations(i, ctr)-1, 1); // subtract 1 (from R back to C indexing!)
			statistics_under_null[ctr] = ComputeStatistic_rcpp(permuted_data, grid_points, expectations_table);
		}
		output["TrueT"] = TrueT;
		output["statistics.under.null"] = statistics_under_null;
		output["Permutations"] = Permutations;
		output["permuted.data"] = permuted_data;
	} // end permutations test
	/**/
	if(test_type == "permutations_inverse_weighting") // Use inverse-weighing Hoeffding's statistic, with MCMC to give pvalue
	{
		NumericMatrix w_mat = w_fun_to_mat_rcpp(data, w_fun);
		TrueT = ComputeStatistic_w_rcpp(data, grid_points, w_fun); //  $Statistic
				
		List PermutationsList = PermutationsMCMC_rcpp(w_mat, prms); // burn.in = prms$burn.in, Cycle = prms$Cycle)
		NumericMatrix Permutations = PermutationsList["Permutations"];
		NumericMatrix permuted_data(n, 2);

		// Compute the statistics value for each permutation:
		NumericVector statistics_under_null(B);
//		NumericMatrix w_mat_permutation(n, n);
		for (ctr = 0; ctr < B; ctr++)
		{
			permuted_data(_, 0) = data(_, 0);
			for (i = 0; i < n; i++)
				permuted_data(i, 1) = data(Permutations(i, ctr)-1, 1); // subtract 1 (from R back to C indexing!)
//			permuted_sample = cbind(data(_, 0), data(Permutations(_, ctr), 1));
//			w_mat_permutation = w_fun_to_mat_rcpp(permuted_data, w_fun);
			NullT = ComputeStatistic_w_rcpp(permuted_data, grid_points, w_fun);
			statistics_under_null[ctr] = NullT;
		}
		output["TrueT"] = TrueT;
		output["statistics.under.null"] = statistics_under_null;
		output["Permutations"] = Permutations;
	} // end permutations with inverse weighting test



	/**/
	if (test_type == "uniform_importance_sampling") { // Our modified Hoeffding's statistic with importance sampling uniform permutations test (not working yet)
		NumericMatrix permuted_data(n, 2);
		NumericMatrix w_mat = w_fun_to_mat_rcpp(data, w_fun); 
		List PermutationsList = PermutationsMCMC_rcpp(w_mat, prms); //
		NumericMatrix Permutations = PermutationsList["Permutations"];
		NumericMatrix P = PermutationsList["P"];
		NumericMatrix expectations_table(1, 1); // set empty (0) table 
		if(!(PL_expectation || naive_expectation))
			expectations_table = QuarterProbFromPermutations_rcpp(data, P, grid_points);
/**
		Rcout << "Before analytic, expected:" << endl; 
		for(i=0; i < 5; i++)
		{
			for(j=0; j < 4; j++)
				Rcout << expectations_table(i, j) << ", ";
			Rcout << endl; 
		} **/
		// New: set analytic expectations table
		expectations_table = double(n) * QuarterProbGaussianAnalytic_rcpp(data, w_fun);  // TEMP FOR DEBUGGING. ONLY FOR GAUSSIAN DEPENDENCIES
/**		Rcout << "SET ANALYTIC EXPECTATIONS TABLE" << endl; 
		Rcout << "After analytic, expected:" << endl; 
		for(i=0; i < 5; i++)
		{
			for(j=0; j < 4; j++)
				Rcout << expectations_table(i, j) << ", ";
			Rcout << endl; 
		}
**/
//		List TrueTList = TIBS_steps_rcpp(data, w_fun, w_mat, grid_points, expectations_table, prms);   // compute statistic. Use permutations for expected table 
//		TrueT = TrueTList["Statistic"];
		permuted_data(_, 0) = data(_, 0);
		for (i = 0; i < n; i++)
			permuted_data(i, 1) = data(Permutations(i, 0)-1, 1); // save one example
		output = IS_permute_rcpp(data, grid_points, B, w_fun, expectations_table);} // instead of w_fun we should give expectation table 


// R code below
/**	           w.mat=w_fun_to_mat(data, w.fun)
           if(prms$use.cpp)  # permutations used here only to determine expected counts for the test statistic 
             Permutations <- PermutationsMCMC_rcpp(w.mat, prms)
           else
             Permutations <- PermutationsMCMC(w.mat, prms)
           P=Permutations$P
           Permutations=Permutations$Permutations
           if((!prms$naive.expectation) & (!prms$PL.expectation))
           {
             if(prms$use.cpp)
               expectations.table <- QuarterProbFromPermutations_rcpp(data, P, grid.points)  # Permutations
             else
               expectations.table <- QuarterProbFromPermutations(data, P, grid.points)
           } else
             expectations.table <- c()
           TrueT <- TIBS.steps(data, w.fun, w.mat, grid.points, expectations.table, prms)  # compute statistic. Use permutations for expected table 
           permuted.data <- cbind(data[,1], data[Permutations[,1],2]) # save one example 
           
           output <- IS.permute(data, prms$B, w.fun, TrueT$expectations.table) # W)  # ComputeStatistic.W(dat, grid.points, w.fun) **/

	if (test_type == "uniform_importance_sampling_inverse_weighting") { // inverse-weighting Hoeffding's statistic with importance sampling uniform permutations  
		NumericMatrix expectations_table(1, 1); // set empty (0) table 
		output = IS_permute_rcpp(data, grid_points, B, w_fun, expectations_table); // need to change!
	} // w = function(x) { 1 }) {
	if(test_type == "tsai") { cout << "Can't run Tsai's test from cpp" << endl; } //   Tsai's test, relevant only for truncation W(x,y)=1_{x<=y}		
	if(test_type == "minP2") { cout << "Can't run minP2 from cpp" << endl;} // minP2 test, relevant only for truncation W(x,y)=1_{x<=y}


 // results = IS.permute(dat, prms$B, w.fun) # W);  // ComputeStatistic.W(dat, grid_points, w.fun)		
// } // end switch 
/**/
	if (!(output.containsElementNamed("permuted.data")))
		output["permuted.data"] = NULL;
/**/
	if (!(output.containsElementNamed("Pvalue"))) //    ("Pvalue" % in % names(output))) // Compute empirical P - value
	{
//		output["Pvalue"] = length(which(output["statistics_under_null"] >= output["TrueT"])) / B;
		double Pvalue = 0.0; 
		NumericVector statistics_under_null = as<NumericVector>(output["statistics.under.null"]);
		for (i = 0; i < B; i++)
			Pvalue += (statistics_under_null[i] >= as<double>(output["TrueT"]));
		output["Pvalue"] = Pvalue / double(B);
	}

	for (i = 0; i < n; i++) // long(grid_points_arma.n_rows); i++)
	  for (j = 0; j < 2; j++)
	      data(i, j) = new_data(i, j); // copy back !! 

		
	return(output);
}
