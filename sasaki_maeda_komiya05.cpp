#include<bits/stdc++.h>
using namespace std;
#define rep(i,n) for(int i=0;i<(n);i++)
#define drep(i,n) for(int i=(n)-1;i>=0;i++)
using ll = long long;
using ld = long double;
#define INF 2e9

#define x_sp_size 10
#define y_sp_size 10
#define search 100

vector<int> dx = {1,0,-1,0};
vector<int> dy = {0,1,0,-1};

//Point定義
struct Point3{
  double x,y,z;
};

//点データを二次元配列に整理する関数
pair<int,int> built_data(const string filename){
	cout << filename << endl;
  ifstream in(filename);
  double x_tmp,y_tmp,z_tmp;
  vector<Point3> data;
  string str;
  double x_max = -INF, x_min = INF, y_max = -INF, y_min = INF;
	int in_cnt = 0;
  //点データ読み込みおよびデータx,yの最大、最小値を入手
  while(getline(in,str)){
    stringstream ss(str);
    ss >> x_tmp >> y_tmp >> z_tmp;
    data.push_back({x_tmp,y_tmp,z_tmp});
    x_max = max(x_max,x_tmp);
    x_min = min(x_min,x_tmp);
    y_max = max(y_max,y_tmp);
    y_min = min(y_min,y_tmp);
    in_cnt++;
  }

  double x_dis = x_max-x_min, y_dis = y_max-y_min;
  vector<vector<double>> xyz(y_dis+1,vector<double>(x_dis+1,-100));  //z座標保持の二次元配列
  vector<vector<bool>> err(y_dis+1,vector<bool>(x_dis+1,false));  //欠損点および空白部分検知配列
  ll n = (ll)data.size();
  rep(i,n){
    if(data[i].z<=-100) continue;
    xyz[data[i].y-y_min][data[i].x-x_min] = data[i].z;
    err[data[i].y-y_min][data[i].x-x_min] = true;
  }

  //便利そうな関数
  auto bnr = [&](auto a,auto b){
    return ((0<=a && a<=y_dis) && (0<=b && b<=x_dis) && err[a][b]);
  };
  
  //便利ver2.0関数
  auto bnr_v2 = [&](int a,int b,int c,int d,double min_dis,double h){
    if(bnr(a,b) && bnr(c,d)){
      if(min_dis>abs(xyz[a][b]-xyz[c][d])){
        min_dis = abs(xyz[a][b]-xyz[c][d]);
        h = (xyz[a][b]+xyz[c][d])/2;
      }
    }
  };

  //欠損点補完関数
  auto hokan = [&](auto a,auto b){
    double min_dis = INF;
    bool ok = true;
    double h = -1;
    bnr_v2(a+1,b,a-1,b,min_dis,h);
    bnr_v2(a+1,b+1,a-1,b-1,min_dis,h);
    bnr_v2(a+1,b-1,a-1,b+1,min_dis,h);
    bnr_v2(a,b+1,a,b-1,min_dis,h);
    if(h==-1){
      ld sum = 0;
      int cnt = 0;
      for(int i=-1;i<=1;i++){
        for(int j=-1;j<=1;j++){
          if(!i && !j) continue;
          if(bnr(a+i,b+j)){
            sum += xyz[a+i][b+j];
            cnt++;
          }
        }
      }
      if(cnt==0){
        ok = false;
        h = 0;
      }
      else h = sum/cnt;
    }
    pair<bool,double> res = {ok,h};
    return res;
  };

  //欠損点補完関数呼び出し
  rep(i,y_dis+1){
    rep(j,x_dis+1){
      if(!err[i][j]){
        pair<bool,double> hkn = hokan(i,j);
        if(hkn.first){
          xyz[i][j] = hkn.second;
          err[i][j] = true;
        }
        else xyz[i][j] = hkn.second;
      }
    }
  }
/*
  rep(i,y_dis+1){
  	rep(j,x_dis+1){
  		int jdg_cnt = 0;
  		for(int ni=-1;ni<=1;ni++){
  			for(int nj=-1;nj<=1;nj++){
  				if(!ni && !nj) continue;
  				if(!bnr(i+ni,j+nj)) continue;
  				if(fabs(xyz[i][j]-xyz[i+ni][j+nj])>1.5) jdg_cnt++;
  			}
  		}
  		if(jdg_cnt>=8){
  			pair<bool,double> hkn = hokan(i,j);
  			xyz[i][j] = hkn.second;
  		}
  	}
  }
*/
  int x_sp = (x_dis+x_sp_size)/x_sp_size, y_sp = (y_dis+y_sp_size)/y_sp_size;

  /*//地面建物判定上から
  auto jt_jdg_lower = [&](double z,pair<int,int> p){

    auto jt_loop_lower = [&](pair<int,int> q){
      int ad = q.first, bd = q.second;
      bool tf = false;
      pair<int,int> min_p = {ad,bd};
      double min_z = xyz[ad][bd];
      for(int i=-search;i<=search;i++){
        for(int j=-search;j<=search;j++){
          if(!bnr(ad+i,bd+j)) continue;
          rep(ni,4){
            if(!bnr(ad+i+dx[ni],bd+j+dy[ni])) continue;
            if(xyz[ad+i+dx[ni]][bd+j+dy[ni]]-xyz[ad+i][bd+j]<-2){
              if(xyz[ad+i+dx[ni]][bd+j+dy[ni]]<min_z){
                min_z = xyz[ad+i+dx[ni]][bd+j+dy[ni]];
                min_p = {ad+i+dx[ni],bd+j+dy[ni]};
                tf = true;
              }
            }
          }
        }
      }
      pair<bool,pair<int,int>> res = {tf,min_p};
      return res;
    };

    bool tf = true;
    pair<int,int> min_pair = p;
    pair<bool,pair<int,int>> res = {tf,min_pair};
    while(res.first){
      res = jt_loop_lower(min_pair);
      min_pair = res.second;
    }
    return res.second;
  };
  
  //地面建物判定下から
  auto jt_jdg_upper = [&](double z,pair<int,int> p){

    auto jt_loop_upper = [&](pair<int,int> q){
      int ad = q.first, bd = q.second;
      bool tf = false;
      pair<int,int> max_p = {ad,bd};
      double max_z = xyz[ad][bd];
      for(int i=-search;i<=search;i++){
        for(int j=-search;j<=search;j++){
          if(!bnr(ad+i,bd+j)) continue;
          rep(ni,4){
            if(!bnr(ad+i+dx[ni],bd+j+dy[ni])) continue;
            if(xyz[ad+i+dx[ni]][bd+j+dy[ni]]-xyz[ad+i][bd+j]<2){
              if(xyz[ad+i+dx[ni]][bd+j+dy[ni]]>max_z){
                max_z = xyz[ad+i+dx[ni]][bd+j+dy[ni]];
                max_p = {ad+i+dx[ni],bd+j+dy[ni]};
                tf = true;
              }
            }
          }
        }
      }
      pair<bool,pair<int,int>> res = {tf,max_p};
      return res;
    };

    bool tf = true;
    pair<int,int> max_pair = p;
    pair<bool,pair<int,int>> res = {tf,max_pair};
    while(res.first){
      res = jt_loop_upper(max_pair);
      max_pair = res.second;
    }
    return res.second;
  };*/

  //k平均法による区画ごとの最高高度、最低高度
  vector<vector<double>> sp_max_z(y_sp_size,vector<double>(x_sp_size,0));
  vector<vector<pair<int,int>>> sp_max_xy(y_sp_size,vector<pair<int,int>>(x_sp_size));
  vector<vector<double>> sp_min_z(y_sp_size,vector<double>(x_sp_size,INF));
  vector<vector<pair<int,int>>> sp_min_xy(y_sp_size,vector<pair<int,int>>(x_sp_size));
  rep(i,y_dis+1){
    rep(j,x_dis+1){
      if(sp_max_z[i/y_sp][j/x_sp]<xyz[i][j]){
        sp_max_z[i/y_sp][j/x_sp] = xyz[i][j];
        sp_max_xy[i/y_sp][j/x_sp] = {i,j};
      }
      if(sp_min_z[i/y_sp][j/x_sp]>xyz[i][j]){
        sp_min_z[i/y_sp][j/x_sp] = xyz[i][j];
        sp_min_xy[i/y_sp][j/x_sp] = {i,j};
      }
    }
  }

  vector<vector<int>> building(y_dis+1,vector<int>(x_dis+1,0));
  vector<vector<bool>> v(y_dis+1,vector<bool>(x_dis+1,false));
  queue<pair<int,int>> que;

    //地面建物判定BFS
  rep(i,y_sp_size){
    rep(j,x_sp_size){
			pair<int,int> grd;
      /*pair<int,int> grd = jt_jdg_lower(sp_max_z[i][j],sp_max_xy[i][j]);
      building[grd.first][grd.second] = 1;
      que.push(grd);
      grd = jt_jdg_upper(sp_min_z[i][j],sp_min_xy[i][j]);
      building[grd.first][grd.second] = 1;
      que.push(grd);*/
      grd = sp_min_xy[i][j];
      building[grd.first][grd.second] = 1;
      que.push(grd);
      while(!que.empty()){
        pair<int,int> f = que.front();
        int a = f.first, b = f.second;
        que.pop();
        for(int i=-1;i<=1;i++){
          for(int j=-1;j<=1;j++){
            if(!bnr(a+i,b+j)) continue;
            if(v[a+i][b+j]) continue;
            v[a+i][b+j] = true;
            if(fabs(xyz[a][b]-xyz[a+i][b+j])<1.2){
              building[a+i][b+j] = 1;
              que.push({a+i,b+j});
            }
          }
        }
      }
    }
  }
  vector<vector<int>> bf_building = building;
  
  // smoothing関数：隣接点のZ値が1メートル以上の差がある点を削除
  for(int i = 0; i < y_dis; i++) {
    for(int j = 0; j < x_dis; j++) {
        if(j != 0 && j != x_dis - 1) { // jがx_disまで到達しないように修正
            int dcnt = 0;
            int gcnt = 0;
            for(int xcnt = -1; xcnt <= 1; xcnt++) {
                for(int ycnt = -1; ycnt <= 1; ycnt++) {
                    if(i+xcnt>=0 && i + xcnt < y_dis && j + ycnt >= 0 && j + ycnt < x_dis) {
                        double zd = std::fabs(xyz[i][j] - xyz[i + xcnt][j + ycnt]);
                        if(zd >= 1.0) {
                            dcnt++;
                        }
                        if(building[i + xcnt][j + ycnt] == 1){
                        	gcnt++;
                        }
                    }
                }
            }
            if(dcnt >= 5) {
                building[i][j] = -1;
            }
            // 地面キレイキレイ
            else if(building[i][j] == 0 and gcnt >= 4){
            	building[i][j] = 1;
            }
        }
    }
  }

  rep(i,y_dis+1){
    rep(j,x_dis+1){
      if(building[i][j]==-1) continue;
      int which = bf_building[i][j];
      int jdg_cnt = 0;
      for(int ni=-1;ni<=1;ni++){
      	for(int nj=-1;nj<=1;nj++){
      		if(!bnr(i+ni,j+nj)) continue;
      		if(which!=bf_building[i+ni][j+nj]) jdg_cnt++;
      	}
      }
      if(jdg_cnt>6){
      	pair<bool,double> hkn = hokan(i,j);
      	xyz[i][j] = hkn.second;
      	building[i][j] = which^1;
      }
    }
  }

  ofstream outputfile;
  std::string opfilename = "Points.txt";
  outputfile.open(opfilename, ios::app);
  int out_cnt = 0;
  
  //外周点判定
  rep(i,y_dis){
    rep(j,x_dis){
      if(building[i][j]%2==0 && building[i][j+1]==1){
      	bf_building[i][j+1] = 1;
      	outputfile << i+y_min << ' ' << j+1+x_min+0.1 << ' ' << xyz[i][j+1] << ' ' << 2 << "\n";
      	building[i][j+1] = -1;
      	out_cnt++;
      }
      if(building[i][j]%2==1 && building[i][j+1]%2==0){
      	bf_building[i][j] = 1;
      	outputfile << i+y_min << ' ' << j+1+x_min+0.1 << ' ' << xyz[i][j] << ' ' << 2 << "\n";
      	building[i][j] = -1;
      	out_cnt++;
      }
    }
  }
  rep(i,x_dis){
    rep(j,y_dis){
      if(building[j][i]%2==0 && building[j+1][i]==1){
      	bf_building[j][i] = 1;
      	outputfile << j+y_min+0.1 << ' '  << i+x_min << ' ' << xyz[j+1][i] << ' ' << 2 << "\n";
      	building[j][i] = -1;
      	out_cnt++;
      }
      if(building[j][i]%2==1 && building[j+1][i]%2==0){
      	bf_building[j+1][i] = 1;
      	outputfile << j+1+y_min+0.1 << ' ' << i+x_min << ' ' << xyz[j][i] << ' ' << 2 << "\n";
      	building[j+1][i] = -1;
      	out_cnt++;
      }
    }
  }
  
 //地面もっとキレイキレイ
  rep(i,y_dis+1){
  	rep(j,x_dis+1){
  		if(building[i][j]==-1) continue;
  		int ard_cnt = 0;
  		double ard_sum = xyz[i][j];
  		for(int ni=-1;ni<=1;ni++){
  			for(int nj=-1;nj<=1;nj++){
  				if(!ni && !nj) continue;
  				if(!bnr(i+ni,j+nj)) continue;
  				if(fabs(xyz[i][j]-xyz[i+ni][j+nj]<1) && building[i+ni][j+nj]>=0){
  					ard_cnt++;
  					ard_sum += xyz[i+ni][j+nj];
  				}
  			}
  		}
  		if(ard_cnt>7){
  			xyz[i][j] = ard_sum/(ard_cnt+1);
  			for(int ni=-1;ni<=1;ni++){
  				for(int nj=-1;nj<=1;nj++){
  					if(!ni && !nj) continue;
  					if(!bnr(i+ni,j+nj)) continue;
  					if(fabs(xyz[i][j]-xyz[i+ni][j+nj]<1) && building[i+ni][j+nj]>=0) building[i+ni][j+nj] = -1;
  				}
  			}
  		}
  	}
  }
  //データ削減関数
  auto del = [&](auto a,auto b){
    if(building[a][b]<0) return;
    rep(i,2){
      rep(j,2){
        //判定基準データ保護のため
        if(!i && !j) continue;
        if(!bnr(a+i,b+j)) continue;
        if(abs(xyz[a][b]-xyz[a+i][b+j])<=1) building[a+i][b+j] = -1;
      }
    }
  };

  //データ削減呼び出し
  for(int i=0;i<y_dis;i+=2){
    for(int j=0;j<x_dis;j+=2){
      del(i,j);
    }
  }
  rep(i,y_dis+1){
    rep(j,x_dis+1){
      if(building[i][j]==-1) continue;
      outputfile << i+y_min << ' ' << j+x_min << ' ' << xyz[i][j] << ' ' << building[i][j] << "\n";
      out_cnt++;
    }
  }
  outputfile.close();
  cout << out_cnt << endl;
  cout << "削減率 ： " << (1-(double)out_cnt/in_cnt)*100 << "％" << endl;
  pair<int,int> res = {in_cnt,out_cnt};
  return res;
}

//本文
int main(){
  ifstream infile;
  infile.open("filename.txt");
  string filename;
  ll in_sum = 0, out_sum = 0;
  while(getline(infile,filename)){
    pair<int,int> sum = built_data(filename);
    in_sum += sum.first;
    out_sum += sum.second;
  }
  infile.close();
  cout << "入力点数：　" << in_sum << endl;
  cout << "出力点数：　" << out_sum << endl;
  cout << "全体削減率：　" << 100*(1-(double)out_sum/in_sum) << "％" << endl;
}
