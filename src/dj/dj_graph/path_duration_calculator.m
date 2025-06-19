clear
clc

syms t1 t2 aa ad v0 D d1 v1
eqn1 = v0+(aa*t1-ad*(t2-t1))/1000 == 0
eqn2 = d1 == 1/2*aa*t1^2/(1000)^2 + v0*t1/1000
eqn3 = v1 == aa*t1/1000
eqn4 = D == -1/2*ad*(t2-t1)^2/(1000)^2 + v1*(t2-t1)/1000 + d1
eqn6 = t1 >= 0
eqn7 = t2 > 0
eqn8 = aa > 0
eqn9 = ad > 0
eqn10 = D > 0
eqn11 = v0 >= 0
eqns = [eqn1, eqn2, eqn3, eqn4, eqn6, eqn7, eqn8, eqn9, eqn10, eqn11]
S=solve(eqns, t1, t2, d1, v1)
S.t2
Simpl = simplify(S.t2)