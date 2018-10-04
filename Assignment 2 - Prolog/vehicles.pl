/*
  Stephen Albrecht, Ryan Gray
  This program helps you decide which type of vehicle to buy based on your needs.  
  start with ?-go.
*/

go :-hypothesize(Vehicle),
  write('You should buy a: '),
  write(Vehicle),
  nl,
  undo.

/* hypotheses to be tested */
hypothesize(pickup_truck) :- pickup_truck, !.
hypothesize(sports_car) :- sports_car, !.
hypothesize(minivan) :- minivan, !.
hypothesize(suv) :- suv, !.
hypothesize(sedan) :- sedan, !.
hypothesize(compact) :- compact, !.
hypothesize(bike). /* no conclusion */

/* car identification rules */
pickup_truck :- cool_car, verify(good_storage_space).
sports_car :- cool_car.
minivan :- utility, verify(room_for_more_than_5_people).
suv :- utility, verify(good_gas_mileage).
sedan :- verify(good_storage_space).
compact :- verify(good_gas_mileage).

/* classification rules */
utility :- verify(good_storage_space).
cool_car :- verify(to_be_a_mean_lookin_machine), !.
cool_car :- verify(at_most_2_seats).


/* how to ask questions */
ask(Question) :-
  write('Does your desired vehicle need: '),
  write(Question), 
  write('? '),
  read(Response),
  nl,
  ((Response == yes ; Response == y)
    ->
    asserta(yes(Question)) ;
    asserta(no(Question)), fail).
    
:-dynamic(yes/1,no/1).

/* how to verify something */
verify(S) :-
  (yes(S) -> true ;
  (no(S) -> fail ;
  ask(S))).

/* at the end, undo all yes or no assertions */
undo :-retract(yes(_)),fail. 
undo :-retract(no(_)),fail.
undo.