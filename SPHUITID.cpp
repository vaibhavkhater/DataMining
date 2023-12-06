#include<bits/stdc++.h>
#define ll long long
#define rep(i,a,b) for(int i=a;i<b;i++)
#define rrep(i,a,b) for(int i=a;i>=b;i--)
#define repin rep(i,0,n)
#define di(a) int a;cin>>a;
#define precise(i) cout<<fixed<<setprecision(i)
#define vi vector<int>
#define si set<int>
#define mii map<int,int>
#define take(a,n) for(int j=0;j<n;j++) cin>>a[j];
#define give(a,n) for(int j=0;j<n;j++) cout<<a[j]<<' ';
#define vpii vector<pair<int,int>>
#define sis string s;
#define sin string s;cin>>s;
#define db double
#define be(x) x.begin(),x.end()
#define pii pair<int,int>
#define pb push_back
#define pob pop_back
#define ff first
#define ss second
#define lb lower_bound
#define ub upper_bound
#define bpc(x) __builtin_popcountll(x) 
#define btz(x) __builtin_ctz(x)
using namespace std;

int power( int N, int M){
    int power = N, sum = 1;
    if(N == 0) sum = 0;
    while(M > 0){if((M & 1) == 1){sum *= power;}
    power = power * power;M = M >> 1;}
    return sum;
}

const long long INF=1e6;
const long long M=1e9+7;
const long long MM=998244353;  

int n;//number of items assuming 0 based indexing
int m;//number of transactions in database
vi eu;//external utuility
map<pii,int> iu;//internal utility
vector<vector<int>> t_id;//stores transaction id n which particular item is occuring
vector<vector<int>> t;//list to store the transactions (also containing the internal utilities)
vector<set<int>> st;//set to store the transactions (also containing the internal utilities)
vi tu;//transaction utility
float delta;//adjustable 
float threshold;//adjustable
float theta;//adjustable 
int invtheta;
vector<bool> rem;
vector<bool> cn;

int item_utility(int item,int t_id){
    return eu[item]*iu[{item,t_id}];
}

bool itemset_present(vector<int> itemset,int t_id){
    bool b=1;
    for(auto x : itemset){
        if(st[t_id].count(x))continue;
        b=0;
    }
    if(b==0)return 0;
    return 1;
}

int itemset_utility_in_transaction(vector<int> itemset,int t_id){
    if(itemset_present(itemset,t_id)==0)return 0;
    int res = 0;
    for(auto x : itemset){
        res+=item_utility(x,t_id);
    }
    return res;
}

int itemset_utility_in_database(vector<int> itemset){
    int res = 0;
    rep(i,0,m){
        res+=itemset_utility_in_transaction(itemset,i);
    }
    return res;
}

int transaction_utility(int t_id){
    int res = 0;
    for(auto x : t[t_id]){
        res+=item_utility(x,t_id);
    }
    return res;
}

int total_utility(){
    int res = 0;
    rep(i,0,m){
        res+=transaction_utility(i);
    }
    return res;
}

bool high_utility_itemset(vector<int> itemset){
    if(itemset_utility_in_database(itemset)>=threshold)return true;
    return false;
}

void print_all_HUI(){
    cout<<"High utility itemsets : "<<"\n";
    int x = power(2,n);
    rep(i,1,x){
        vector<int> itemset;
        int y=1;
        int cnt=0;
        rep(j,0,n){
            if((i&y)!=0){itemset.pb(cnt);}
            cnt++;
            y*=2;
        }
        cnt = 8;
        if(high_utility_itemset(itemset)){
            cout<<"{";
            rep(j,0,itemset.size()){
                if(j==itemset.size()-1){cout<<itemset[j];cnt-=1;continue;}
                cout<<itemset[j]<<",";cnt-=2;
            }
            cout<<"} ";
            rep(j,0,cnt){cout<<" ";}cout<<":  ";
            cout<<itemset_utility_in_database(itemset)<<"\n";
        }
    }
    cout<<"\n";
}

vector<int> high_utility_transaction_list(vector<int> itemset){
    vi T;
    rep(i,0,m){
        if((!rem[i]) && itemset_present(itemset,i) && itemset_utility_in_transaction(itemset,i)>=threshold){
            T.pb(i+1);
        }
    }
    return T;
}

vector<int> Transaction_ID_list(vector<int> itemset){
    vi T;
    rep(i,0,m){
        if((!rem[i]) && itemset_present(itemset,i)){
            T.pb(i+1);
        }
    }
    return T;
}

vector<int> period_of_an_itemset(vector<int> itemset){
    vi T = high_utility_transaction_list(itemset);
    vi P;
    if(T.size()==0)return P;
    P.pb(T[0]);
    rep(i,1,T.size()){
        P.pb(T[i]-T[i-1]);
    }
    P.pb(m+1-T.back());
    return P;
}

int maximum_period_of_an_itemset(vector<int> itemset){
    int mx = -INF;
    vi P = period_of_an_itemset(itemset);
    if(P.size()==0)return INF;
    for(auto x : P)mx=max(mx,x);
    return mx;
}

bool short_period_high_utility_itemset(vector<int> itemset){
    if((float)maximum_period_of_an_itemset(itemset)<=(float)theta*m)return true;
    else return false;
}

int transaction_weighted_utility_of_an_itemset(vector<int> itemset){
    int res = 0;
    rep(i,0,m){
        if(itemset_present(itemset,i)){
            res+=transaction_utility(i);
        }
    }
    return res;
}

bool high_transaction_weighted_high_utility_itemset(vector<int> itemset){
    if(transaction_weighted_utility_of_an_itemset(itemset)>=threshold)return true;
    return false;
}

bool sphuiMT(vector<int> itemset){
    vi TID = Transaction_ID_list(itemset);
    if(TID.size()==0)return 0;
    vi P;
    P.pb(TID[0]);
    rep(i,1,TID.size()){
        P.pb(TID[i]-TID[i-1]);
    }
    P.pb(m+1-TID.back());
    int mx = P[0];
    for(auto x : P){
        mx=max(mx,x);
    }
    if(mx<=theta*m)return true;
    else return false;
}

void SPHUITID(){
    rep(i,0,m){
        if(tu[i]<threshold)rem[i]=true;
        else rem[i]=false;
    }
}

void print_short_phase_high_utility_itemset_mining(){
    vi htwuspi;
    repin{
        int y = power(2,i);
        vi v1;v1.pb(i);
        int fu = transaction_weighted_utility_of_an_itemset(v1);
        if(fu >= threshold*invtheta){
            htwuspi.pb(y);
            cn[y]=true;
        }
    }
    vi temp=htwuspi;
    while(temp.size()){
        vi temp1;
        rep(i,0,temp.size()){
            rep(j,i+1,temp.size()){
                int res = temp[i]|temp[j];
                if((!cn[res])&&(bpc(res)==bpc(temp[i])+1)){
                    int x=1;
                    vi v1;
                    rep(k,0,n){
                        if(res&x)v1.pb(k);
                        x*=2;
                    }
                    int fu = transaction_weighted_utility_of_an_itemset(v1);
                    if(fu >= threshold*invtheta){
                        htwuspi.pb(res);
                        temp1.pb(res);
                        cn[res]=true;
                    }
                }
            }
        }
        temp=temp1;
    }
    vi sphui;
    vi vhtwuspi;
    for(auto y : htwuspi){
        vi v1;   
        int x=1;
        rep(i,0,n){
            if(y&x)v1.pb(i);
            x*=2;
        }
        if(!sphuiMT(v1))continue;
        vhtwuspi.pb(y);
        if(short_period_high_utility_itemset(v1))sphui.pb(y);
    }
    cout<<"High Transaction weighted Utility short period itemsets \n";
    cout<<"succesfully pruned with TID strattegy :\n";
    for(auto y : vhtwuspi){
        vi v1;
        int x=1;
        rep(k,0,n){
            if(y&x)v1.pb(k);
            x*=2;
        }
        // cout<<"{";
        // char c='A';
        // c += v1.back();
        // rep(i,0,v1.size()-1){
        //     char c = 'A';
        //     c+=v1[i];
        //     cout<<c<<",";
        // }cout<<c<<"}";
        // rep(i,0,10-2*v1.size()-1){
        //     cout<<" ";
        // }cout<<":  ";
        // cout<<transaction_weighted_utility_of_an_itemset(v1)<<"\n";      
    }
    cout<<"\nShort Period High Utility Itemsets "<<":\n";
    for(auto y : sphui){
        vi v1;
        int x=1;
        rep(k,0,n){
            if(y&x)v1.pb(k);
            x*=2;
        }
        // cout<<"{";
        // char c='A';
        // c += v1.back();
        // rep(i,0,v1.size()-1){
        //     char c = 'A';
        //     c+=v1[i];
        //     cout<<c<<",";
        // }cout<<c<<"}";
        // rep(i,0,10-2*v1.size()-1){
        //     cout<<" ";
        // }cout<<":  ";
        // cout<<transaction_weighted_utility_of_an_itemset(v1)<<"\n";      
    }

}

void solve()
{
    cin>>n;
    eu.resize(n);
    take(eu,n);
    t_id.resize(n);
    cin>>m;
    t.resize(m);
    st.resize(m);
    cn.resize(1<<n);
    rem.resize(m);
    rep(i,0,m){
        rep(j,0,n){
            int x = j;
            di(v);
            if(v==0)continue;
            t[i].pb(x);
            st[i].insert(x);
            iu[{x,i}]=v;
            t_id[x].pb(i);
        }
    }
    tu.resize(m);
    rep(i,0,m){
        tu[i]=transaction_utility(i);
    }
    delta = 0.02;
    threshold=total_utility()*delta;
    theta = 0.4;
    float r = 1.0/theta;
    invtheta = r;
    SPHUITID();
    cout<<"threshold        : "<<threshold<<"\n\n";
    cout<<"delta_threshhold : "<<delta<<"\n\n";
    cout<<"period_threshold : "<<theta<<"\n\n";
    print_short_phase_high_utility_itemset_mining();
}

signed main(){
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    auto start = chrono::high_resolution_clock::now();
    freopen("dataset3.txt", "r", stdin);
    // freopen("data2.txt", "w", stdout);
    #ifdef NCR
        init();
    #endif
    #ifdef SIEVE
        sieve();
    #endif
        solve();
    auto end = chrono::high_resolution_clock::now();
    double time_taken =
      chrono::duration_cast<chrono::nanoseconds>(end - start).count(); 
    time_taken *= 1e-9;
    cout << "Time taken by program is : " << fixed
         << time_taken << setprecision(9);
    cout << " sec" << endl;
    return 0;
}