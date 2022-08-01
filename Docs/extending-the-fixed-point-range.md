# FixedPointの範囲を拡張した場合に必要となるShareCompとPrimeFieldの修正点について

## 背景

- オーバフロー対策として、FixedPointのvalueの値をint128, int256等に拡張する可能性がある。
    - 個人的には、int256がいいかなと思います(理由は後述)。
- その際に、ShareCompとPrimeFieldも修正が必要となる。
- その修正点について整理する

## 提案

### PrimeFieldの修正

(1) `value`の型を修正

FixedPointのvalueの値をint128にする場合は，`boost::multiprecision::uint256_t` に。

FixedPointのvalueの値をint256にする場合は，`boost::multiprecision::uint512_t` に。

乗算した時にオーバフローしないように、2倍のビット長が必要になります。

演算ごとにmod pをとる仕様なので、オーバフローしないです。

(2) クラス定数を修正

- `p`
    - FixedPointのvalueの値をint128にする場合は，$2^{128}$ 未満の最大の素数を設定
        - $p=2^{128} - 159 = 340282366920938463463374607431768211297$
    - FixedPointのvalueの値をint256にする場合は，$2^{256}$ 未満の最大の素数を設定
        - $p=2^{256} - 189 = 115792089237316195423570985008687907853269984665640564039457584007913129639747$
- `l`
    - FixedPointのvalueの値をint128にする場合は，`l=128`　に。
    - FixedPointのvalueの値をint256にする場合は，`l=256`　に。
- `lambda`
    - `l` の平方根をとる。
    - FixedPointのvalueの値をint256にする場合は，`lambda=16`　に。
    - FixedPointのvalueの値をint128にすると、lambdaが整数でなくなる。したがって工夫が必要となる。
        - **これを避けるためにも、int256の方が好ましい。**
- `inverse_of_two`
    - mod p における2の逆元
    - FixedPointのvalueの値をint128にする場合は，170141183460469231731687303715884105649
    - FixedPointのvalueの値をint256にする場合は，57896044618658097711785492504343953926634992332820282019728792003956564819874


(3) 関数の引数の型、返り値の型、関数内の変数の型も上記に応じて修正する。

たくさんあるので、漏れなく修正する。

(今後も修正されることを考えると、インタフェース化しておく方が良いかも？)

(4) テストを修正する

型の修正と値の修正

(5) （もし存在すれば）プログラム中にハードコーディングされている値を修正する。

### ShareCompの修正

(1) **subUnboundedFanInOr, Andで使用する(l+1)次多項式の係数を計算する**

現状、f_65_or_coefficient, f_65_and_coefficientとなっている部分を、

- FixedPointのvalueの値をint128にする場合は，f_129_or_coefficient, f_129_and_coefficientに、
- FixedPointのvalueの値をint256にする場合は，f_257_or_coefficient, f_257_and_coefficient

に修正する。

やることは以下の通り(Orの場合)。

$f_l(1)=0, \ f_l(2)=f_l(3)= ... = f_l(l+1)=1$ を満たす$l$次多項式$f_l(x)=\alpha_0+\alpha_1x+...+\alpha_lx^l \bmod p$ を定義する。

- $f_l(x)$ はラグランジュの補間多項式を使用して求める。

Andの場合は，f(1) = f(2) = ... = f(l) = 0, f(l+1) = 1となります。

各係数の値については、一番下に記述してあります。

(2) 関数の引数の型、返り値の型、関数内の変数の型を修正する。

boost::multiprecision::uint128_tになっている部分を、boost::multiprecision::uint256_tやboost::multiprecision::uint512_tに修正する。

(3) テストを修正する

型の修正と値の修正

(4) （もし存在すれば）プログラム中にハードコーディングされている値を修正する。
