% This LilyPond file was generated by Rosegarden 15.12
\include "nederlands.ly"
\version "2.12.0"
\header {
    composer = "Alexandre Glazunov (1865 - 1936)"
    copyright = "Public domain"
    opus = "71"
    title = "Chant du Ménéstrel"
    tagline = "Created using Rosegarden 15.12 and LilyPond"
}
#(set-global-staff-size 18)
#(set-default-paper-size "a4")
global = { 
    \time 4/4
    \skip 1*62 
}
globalTempo = {
    \override Score.MetronomeMark #'transparent = ##t
    \tempo 4 = 76  \skip 1*32 
    \tempo 4 = 96  \skip 1*16 
    \tempo 4 = 76  
}
\score {
    << % common
        % Force offset of colliding notes in chords:
        \override Score.NoteColumn #'force-hshift = #1.0
        % Allow fingerings inside the staff (configured from export options):
        \override Score.Fingering #'staff-padding = #'()

        \context Staff = "track 4, Cello" << 
            \set Staff.instrumentName = \markup { \center-column { "Cello " } }
            \set Staff.midiInstrument = "Cello"
            \set Score.skipBars = ##t
            \set Staff.printKeyCancellation = ##f
            \new Voice \global
            \new Voice \globalTempo

            \context Voice = "voice 1" {
                % Segment: Cello (PIANO CUE)
                \override Voice.TextScript #'padding = #2.0
                \override MultiMeasureRest #'expand-limit = 1
                \tiny
                \once \override Staff.TimeSignature #'style = #'() 
                \time 4/4
                
                \clef "tenor"
                \key a \major
                R1*3^\markup { \bold \large "Lento" }   |
                r2 r4 cis' -\mp  |
%% 5
                a' 2. ^\markup { \bold "dolce ed appassionato" } _~ a' 8 [ gis' 16 fis' ]  |
                gis' 2. cis' 4  |
                fis' 2 _~ fis' 8 [ e' 16 ^( d' ] cis' [ b cis' d' ) ]  |
                e' 2. e' 8 ^( [ fis' ) ]  |
                d' 4. ^( e' 8 ) cis' 4 \< cis' 8 ^( [ a' ) ]  |
%% 10
                a' 8 \! ^( [ b' ) ] gis' 4 ^( _~ gis' 8 \> [ a' ) ] fis' 4 ^( _~  |
                fis' 8 -\p \! [ dis' ) \< ] e' 4 ^( _~ e' 8 ^\markup { \bold "allargando poco" } [ cis' ) ] d' 4  |
                c' 8 -\f \! ^( [ cis' gis' r16 fis' ) ] e' 8 \> ^( [ dis' gis a ) \! ]  |
                b 8 ^\markup { \bold "animando" } ^( \< cis' 4 ) d' 8 -\accent _~ d' \! [ cis' \> ^( gis a ) ]  |
                b 8 \! \< ^( [ cis' e' ) d' -\accent _~ ] d' \! [ cis' \> ^( gis a ) ]  |
%% 15
                b 8 \! \< ^( [ d' gis' fis' ) ] f' ^( [ gis' b' a' ) ]  |
                a' 4 ^\markup { \bold "calando" } \! ^( gis' 2 ) \> cis' 4  |
                a' 2. ^\markup { \bold "Tempo I." } \! gis' 16 ^( [ fis' e' fis' ) ]  |
                gis' 2. cis' 4 ^(  |
                fis' 8 ) r fis' 4 _~ fis' 16 [ a' ^( gis' fis' e' 32 d' cis' b a b cis' d' ) ]  |
%% 20
                e' 2. e' 8 ^( [ fis' ) ]  |
                d' 4. ^( e' 8 ) cis' 4 \< cis' 8 ^( [ a' ) ]  |
                a' 8 \! ^( [ b' ) ] gis' 4 ^( _~ gis' 8 [ a' ) ] fis' 4 \> ^( _~  |
                fis' 8 \! [ dis' ) ] e' 4 ^( _~ e' 8 \< [ cis' ) ] d' 4  |
                gis 4. \! _( ais 8 [ fis ) ] r b 4 -\mf  |
%% 25
                fis' 2 _~ fis' 8 [ e' 16 ^( d' ] \times 4/6 { cis' b cis' d' e' fis' }  |
                gis' 8 ) r16 a' fis' 4 _~ fis' 8 r fis' 4 -\f  |
                b' 2 _~ b' 8 [ a' 16 ^( gis' ] \times 4/6 { fis' e' fis' gis' a' b' }  |
                cis'' 8 ) [ r16 d'' ] b' 2 b' 8 -\tenuto [ cis'' -\tenuto ]  |
                b' 4 \< a' 8 -\tenuto [ b' -\tenuto ] a' 4 gis' 8 -\tenuto [ a' -\tenuto ]  |
%% 30
                gis' 8 -\ff \! [ fis' a -\tenuto b -\tenuto ] cis' 4. \> _~ cis' 16 \! cis'  |
                a' 4. _\markup { \bold \italic "meno" } gis' 16 -\f [ fis' ] gis' 4. \> ^( cis' 8 )  |
                fis' 2 \! _~ fis' 8 r r4  |
                \clef "tenor"
                \key d \major
                a 4 -\mf ^( d' 8 ^\markup { \bold \large "Poco piu mosso" } [ e' ) ] fis' 4. ^( e' 8 )  |
                d' 8 ^( [ cis' b e' ) ] cis' 4 -\tenuto \> ^( a -\tenuto )  |
%% 35
                d' 8 -\p \! ^( [ e' ) fis' -\tenuto g' -\tenuto ] fis' ^( \< [ g' ) ] e' ^( [ fis' ) ]  |
                e' 8 ^( [ fis' ) d' ^( e' ) ] a 2 -\mf -\accent \!  |
                a' 4 ^( g' 8 [ fis' ) ] b' 4. ^( a' 8 )  |
                g' 8 ^( [ fis' e' a' ) ] fis' 2 _~  |
                fis' 8 [ d' ^( \< cis' b ) ] cis' ^( [ fis' cis' a ) ]  |
%% 40
                gis 4. -\f \! _( fis 8 ) \> fis 4 ^( fis' )  |
                g' 4 -\p \! a' 8 [ b' ] a' 4. ^( e' 8 )  |
                g' 8 ^( [ fis' ) e' b' ] a' 4. ^( \< g' 8 )  |
                fis' 8 -\mf \! ^( [ d'' ) \< cis'' -\tenuto b' -\tenuto ] bes' ^( [ b' ) a' -\tenuto g' -\tenuto ]  |
                fis' 8 -\f \! ^( [ b' ) a' -\tenuto g' -\tenuto ] fis' ^( \> [ g' ) fis' -\tenuto e' -\tenuto ]  |
%% 45
                d' 4 -\mf \! ^( g' 8 [ a' ) ] b' 4. ^( a' 8 )  |
                g' 8 ^( [ fis' e' a' ) ] fis' 4. ^( \> e' 8 )  |
                d' 8 -\p \! ^( [ cis' b e' ) ] cis' 4. ^( b 8 )  |
                a 4. ^\markup { \bold "riten." } ^( \< b 8 ) gis 4. \! _( \> fis 8 )  |
                \clef "tenor"
                \key a \major
                fis 8 ^\markup { \bold \large "Tempo I" } \! cis -\p -\staccato _( [ dis -\staccato f -\staccato ) ] fis -\staccato _( [ gis -\staccato ] a 4 -\tenuto )  |
%% 50
                r8 gis -\staccato ^( [ a -\staccato b -\staccato ) ] cis' -\staccato ^( [ d' -\staccato ] e' 4 -\tenuto )  |
                r8 fis -\staccato _( [ gis -\staccato a -\staccato ) ] b -\staccato ^( [ cis' -\staccato ] d' 4 -\tenuto )  |
                r8 b -\staccato ^( [ cis' -\staccato d' -\staccato ) ] e' -\staccato ^( [ fis' -\staccato ) ] g' 4 ^( _~  |
                g' 8 [ e' ) ] fis' 4 ^( _~ fis' 8 [ d' ) ] e' 4  |
                f' 8 ^( \< [ fis' gis' cis'' ) ] b' -\f \! \> ^( [ a' ) ] d' -\p \! [ e' ]  |
%% 55
                cis' 4. ^\markup { \bold "allargando poco" } ^( \< d' 8 ) b 4 b 8 ^( [ g' ) ]  |
                g' 8 -\ff \! ^( [ fis' ) a -\tenuto b -\tenuto ] cis' 4. ^( \> a 8 )  |
                \clef "bass"
                gis 8 -\accent \! ^( [ fis c cis ) ] dis ^( [ f fis -\staccato gis -\staccato ) ]  |
                b 8 -\accent ^( [ a dis f ) ] fis ^( [ gis a -\staccato b -\staccato ) ]  |
                \clef "tenor"
                d' 8 -\accent ^( \< [ cis' gis a ) ] b ^( [ cis' dis' -\staccato f' -\staccato ) ]  |
%% 60
                gis' 2. ^\markup { \bold "rallent." } -\f -\accent \! ^( fis' 4 )  |
                a' 1 _\markup { \bold \italic "dim." } _~  |
                a' 1 -\fermata \>  |
                \bar "|."
            } % Voice
        >> % Staff ends
        \context GrandStaff = "1" <<

            \context Staff = "track 5, Piano" << 
                \set Staff.instrumentName = \markup { \center-column { "Piano " } }
                \set Staff.midiInstrument = "Acoustic Grand Piano"
                \set Score.skipBars = ##t
                \set Staff.printKeyCancellation = ##f
                \new Voice \global
                \new Voice \globalTempo

                \context Voice = "voice 2" {
                    % Segment: Piano - Right Hand
                    \override Voice.TextScript #'padding = #2.0
                    \override MultiMeasureRest #'expand-limit = 1
                    \once \override Staff.TimeSignature #'style = #'() 
                    \time 4/4
                    
                    \clef "treble"
                    \key a \major
                    cis' 4 -\p -\tenuto \< < a cis' a' > < cis'' a' a'' > 2  |
                    dis' 4 -\tenuto < a' a cis' > < cis'' a'' a' > 2  |
                    f' 4 -\mf -\tenuto \! < a a' cis' > < a' a'' cis'' > 2  |
                    e' 2. _( \> _~ e' 8 fis' )  |
%% 5
                    fis' 4 -\p -\tenuto \! < a cis' a' > < a'' cis'' a' > 2  |
                    r4 < gis' cis'' a' cis' > < gis'' cis''' cis'' > 2  |
                    r4 < fis' fis'' a' cis'' > < fis''' fis'' a'' > 2  |
                    r4 < a' cis'' e' a'' > < a'' cis''' a''' > cis' _( _~  |
                    cis' 8 [ ais ) ] b 4 _( \< _~ b 8 [ gis ) ] a 4  |
%% 10
                    < b cis' > 2 -\mf \! \> < cis' a > 4 < a' a'' > 8 \! ^( < b' b'' > -\p )  |
                    < gis'' gis' > 4. ^( \< < a' a'' > 8 ) < fis'' fis' > 4 < fis' fis'' > 8 ^( [ < d'' d''' > ) ]  |
                    < d'' d''' > 8 -\f \! ^( [ < cis'' cis''' > < e' e'' > < fis'' fis' > ) ] < gis' gis'' > 2 \> _~  |
                    < gis'' gis' > 8 \! r r-\mf  < fis' b' > -\accent _( < gis' b' > 4 ) r  |
                    r4 r8 < fis' b' > -\accent _( < b' gis' > 4 ) r  |
%% 15
                    r4 < d' b > 2 -\p \< < b d' > 4  |
                    < d' b > 2 -\mf \! < b cis' > 4 \> r  |
                    r8 \! < a' cis'' > -\p -\staccato [ < cis'' dis'' a' > -\staccato < f'' a' cis'' > -\staccato ] < fis'' a' cis'' > -\staccato [ < a' a'' cis'' > -\staccato ] r4  |
                    r8 < cis'' gis'' > -\staccato [ < gis'' a'' cis'' > -\staccato < b'' cis'' gis'' > -\staccato ] < cis'' gis'' cis''' > -\staccato [ < e'' e''' > -\staccato ] r4  |
                    r8 < cis'' fis'' cis''' > -\staccato [ < d'' fis'' d''' > -\staccato < e'' e''' cis''' > -\staccato ] < fis''' fis'' cis''' > -\staccato [ < a'' a''' > -\staccato ] r4  |
%% 20
                    r8 < e'' cis'' e' > -\staccato [ < fis' cis'' fis'' > -\staccato < gis' cis'' gis'' > -\staccato ] < a' a'' cis'' > -\staccato [ < cis'' cis''' > -\staccato ] cis' 4 _( _~  |
                    cis' 8 [ ais ) ] b 4 _( _~ b 8 \< [ gis ) ] a 4  |
                    < cis' b > 2 \! _( \> < cis' a > 4 ) < a' a'' > 8 -\p \! ^( [ < b'' b' > ) ]  |
                    < gis' gis'' > 4. ^( < a'' a' > 8 ) < fis' fis'' > 4 \< < fis' fis'' > 8 ^( [ < d'' d''' > ) ]  |
                    < d'' fis'' d''' > 8 -\mf \! ^( [ < e'' fis'' e''' > ] < cis'' fis'' cis''' > 4 \> _~ < cis'' fis'' cis''' > 8 [ < d'' fis'' d''' > ] < b' b'' > 4 )  |
%% 25
                    r4 \! < fis' b' b d' > -\mf < d'' fis'' b' fis' > 2  |
                    r4 \times 8/9 { fis'' 32 -\p ^( e'' d'' cis'' b' cis'' d'' e'' fis'' } gis'' 8 ) [ r16 a'' ] fis'' 8 r  |
                    r4 < b' fis' fis'' d'' > -\f < b'' b' d'' fis'' > 2  |
                    r4 \times 8/9 { b'' 32 -\mf ^( a'' gis'' fis'' e'' fis'' gis'' a'' b'' } \stemDown cis''' 8 ) [ r16 d''' ] \stemNeutral b'' 8 r  |
                    r8 < b fis' d' > -\mf r4 \< r8 < fis d' b > r4  |
%% 30
                    < a fis cis' > 2 -\f \! \stemDown < a' cis'' gis'' > 8 -\mf ^( [ < cis'' a' fis'' > ) < fis' a' > < fis' b' a' > ] \stemNeutral  |
                    cis'' 4 ^( dis'' 2 \> f'' 4  |
                    < cis'' fis' fis'' > 4 ) \! < fis'' gis'' d''' > ^( < fis'' fis''' a'' > 8 ) r r4  |
                    \clef "treble"
                    \key d \major
                    < fis' d' a fis > 2 -\mf r8 a' ^( [ b' cis'' ]  |
                    < d'' fis' d' > 4 ) < e' b' e'' > < e' a' cis' > 8 _( \> [ < e' e'' cis' > < cis' fis'' e' > < cis' g'' e' > ) ]  |
%% 35
                    < a' a'' > 4 \! _( < a fis' a' a'' > \< r < a e' g' a'' >  |
                    a'' 4 < b a'' d' fis' > ) < cis' a'' > 8 \! < cis' a' a'' > ^( [ < cis' b' b'' > < cis' cis'' cis''' > ]  |
                    < d'' d''' > 8 ) r r4 d' 8 b' ^( [ < b a' cis'' > < b a' dis'' > ]  |
                    < g' b' e'' > 8 [ < g' b' fis'' > ] < g'' a' e' > ) [ < e' a' > ] < a' fis' > < fis' a' fis'' > ^( [ < gis' e'' gis'' > < bes' e'' bes'' > ]  |
                    < b' d'' b'' > 8 [ < cis'' d'' cis''' > ] < d'' d''' fis'' > 4 ) < cis'' fis'' cis''' > \< < a'' cis''' cis'' > 8 [ < dis''' dis'' fis'' > ]  |
%% 40
                    < b'' cis''' f'' f''' > 4. -\f \! ^( \> < fis''' fis'' > 8 ) < a'' fis''' fis'' cis''' > 4 < a' fis' b > -\p \!  |
                    < b e' g' > 2 _( < a a' e' > 8 ) < a' e'' > ^( < cis'' a'' > 4 )  |
                    < b e' g' > 2 _( < e' a' a > 8 ) < a' e'' > ^( < cis'' a'' > 4 )  |
                    r4-\mf  < d'' fis'' > \< ^( < g' e'' > 4. b' 8  |
                    < fis' d'' > 8 -\f ) \! [ < b' fis'' > ^( ] < b'' d'' > 4 ) < bes d' > 4. \> _( < bes cis' > 8  |
%% 45
                    b 8 ) \! [ < g'' b' > ] < b'' d'' > 4 < b d' > -\mf _( dis' )  |
                    < b e' > 2 < e' b > 8 _( \> [ < d' b > ] < cis' bes > 4 )  |
                    \clef "bass"
                    < fis b > 2 -\p \! < fis b > 8 ^( [ < fis a > ] < gis f > 4 )  |
                    fis 4 ^( \< d ) fis \! \> \clef "treble"
                    cis''  |
                    \key a \major
                    < a' cis'' a'' > 2. -\p \! _~ < a'' cis'' a' > 8 [ < a' cis'' gis'' > 16 ^( < a' fis'' cis'' > ) ]  |
%% 50
                    < b' d'' gis'' > 2 ^( < gis' gis'' cis'' > 4 ) cis''  |
                    < a' cis'' fis'' > 2 ^( < fis'' b' fis' > 8 ) [ e'' 16 ^( d'' ] cis'' [ b' cis'' d'' ) ]  |
                    < e'' g' b' > 2 < a' e'' e' > 4 e'' 8 ^( [ fis'' ) ]  |
                    d'' 4. ^( e'' 8 ) cis'' 4 cis'' 8 ^( \< [ < a' a'' > ) ]  |
                    < a' cis'' a'' > 8 ^( [ < b' b'' cis'' > ) ] < gis' cis'' gis'' > 4 _~ < gis'' cis'' gis' > 8 -\f \! [ < cis'' a' a'' > \> ] \stemDown < b' fis'' b'' > 4 -\p \! _~ \stemNeutral  |
%% 55
                    < b' b'' fis'' > 8 \< [ < fis'' gis'' gis' > ] < a' e'' a'' > 4 _~ < a'' e'' a' > 8 [ < e'' fis' fis'' > ] < g'' g' d'' b' > 4  |
                    < f' a' cis'' f'' > 8 -\f \! ^( [ < fis' a' cis'' fis'' > ] < cis'' a' cis''' > ) [ < cis'' a' > 16 < b' b'' cis'' a' > ] < a' cis'' a'' > 8 \> [ < gis' gis'' cis'' > < cis'' cis' > \stemDown < cis'' dis' dis'' > ] \stemNeutral  |
                    < f' a' cis'' f'' > 8 -\mf -\accent \! ^( < fis' a' cis'' fis'' > 4. _~ < fis'' a' cis'' fis' > 2 )  |
                    < gis' cis'' fis'' gis'' > 8 -\accent ^( < a' cis'' fis'' a'' > 4. _~ < fis'' cis'' a'' a' > 2 )  |
                    < c'' fis'' a'' c''' > 8 -\accent ^( < cis'' fis'' a'' cis''' > 4. _~ < cis''' cis'' a'' fis'' > 2 )  |
%% 60
                    r4 < a' cis'' a'' > -\f < a'' a''' cis''' > 2  |
                    r2 < fis' fis'' cis'' a' > -\p  |
                    < a cis' fis' > 1 -\fermata  |
                    \bar "|."
                } % Voice
            >> % Staff ends

            \context Staff = "track 6, Piano" << 
                \set Staff.instrumentName = \markup { \center-column { "Piano " } }
                \set Staff.midiInstrument = "Acoustic Grand Piano"
                \set Score.skipBars = ##t
                \set Staff.printKeyCancellation = ##f
                \new Voice \global
                \new Voice \globalTempo

                \context Voice = "voice 3" {
                    % Segment: Piano - Left Hand
                    \override Voice.TextScript #'padding = #2.0
                    \override MultiMeasureRest #'expand-limit = 1
                    \once \override Staff.TimeSignature #'style = #'() 
                    \time 4/4
                    
                    \clef "bass"
                    \key a \major
                    r4 < a,, a, > a 2  |
                    r4 < cis cis, > b 2  |
                    r4 < cis, cis > cis' 2  |
                    R1  |
%% 5
                    r4 < fis fis, > < fis a cis' > 2  |
                    r4 < e e, > < gis' cis' e a > 2  |
                    r4 < d, d > < d fis' cis' a > 2  |
                    r4 < a, a,, > < a cis' e' > r  |
                    < b, g > 4 ^( < fis b, > ) < cis f > ^( < fis cis > )  |
%% 10
                    < dis cis, > 4. _( f 8 ) < fis, cis > 4 ^( < cis' fis > )  |
                    < cis cis' gis > 4 ^( < b gis e > ) < b, fis b > ^( < a d fis > )  |
                    < gis, e gis > 4 ^( < gis gis, e cis' > ) < fis c' gis gis, > 2  |
                    < f b gis cis > 4 r8 < fis d' > -\accent ^( < f gis > 4 ) r  |
                    r4 r8 < fis d' > -\accent ^( < f gis > 4 ) r  |
%% 15
                    r4 < a, a > _( < gis, gis > < fis fis, > )  |
                    < f, f > 2 < f, f > 4 r  |
                    r8 < fis a cis' > -\staccato [ < a cis' fis > -\staccato < fis a cis' > -\staccato ] < a cis' fis > -\staccato [ < cis' a fis > -\staccato ] r4  |
                    r8 < a gis' cis' e > -\staccato [ < a gis' e cis' > -\staccato < cis' gis' e a > -\staccato ] < a e gis' cis' > -\staccato [ < cis' gis' e a > -\staccato ] r4  |
                    r8 < d cis' a fis' > -\staccato [ < cis' a fis' d > -\staccato < fis' cis' d a > -\staccato ] < d cis' fis' a > -\staccato [ < a fis' cis' d > -\staccato ] r4  |
%% 20
                    r8 < cis' a e' > [ < cis' a e' > < cis' a e' > ] < a e' cis' > [ < cis' a e' > ] r4  |
                    < b, g > 4 ^( < fis b, > ) < f cis > ^( < fis cis > )  |
                    < dis cis, > 4. _( f 8 ) < fis, cis > 4 ^( < cis' fis > )  |
                    < cis gis cis' > 4 ^( < b e gis > ) < b b, fis > ^( < fis a d > )  |
                    < fis e' > 2 < b, d' fis > 4 r  |
%% 25
                    r4 < gis b, e, > < e gis d' > 2  |
                    r4 < fis d' b, > < b d' fis' > _~ < b d' fis' > 8 r  |
                    r4 < b, gis e, > < e gis d' > 2  |
                    r4 < d' b, fis > < b d' fis' > _~ < b d' fis' > 8 r  |
                    r8 < gis, gis,, > r4 r8 < b, b,, > r4  |
%% 30
                    < cis, cis > 2 r  |
                    < cis' fis' cis'' a' > 2 < cis'' b cis gis' cis' >  |
                    < a cis' fis > 4 \clef "treble"
                    < b' d'' > ^( \stemDown < cis'' fis' > 8 ) r r4 \stemNeutral  |
                    \clef "bass"
                    \key d \major
                    < a, d d, > 2 r4 < cis' a, g >  |
                    < b, fis b > 4 < b g, > < a, a > < g, g >  |
%% 35
                    < fis fis, > 4 < d d, > < cis cis, > < a,, a, >  |
                    < fis, fis,, > 4 < g,, g, > < a,, a, > < a cis' g e' >  |
                    < d' a a' fis > 4 d < g g, > < fis fis, >  |
                    < e, e > 4 < cis, cis > < d, d > < cis cis, >  |
                    < b, b,, > 4 < gis,, gis, > < a, a,, > < b, b,, >  |
%% 40
                    < cis cis, > 4 < cis cis' > < fis, fis > < dis, dis >  |
                    < e, e > 4 < d d, > < cis cis, > < a,, a, >  |
                    < e e, > 4 < d d, > < cis, cis > < a, a,, >  |
                    < d fis a > 4 ^( < d fis bes > 8 [ < d fis b > ) ] < e g cis' > ^( < e g b > 4 < g cis' e > 8 )  |
                    < b d' fis > 4 < b d d' > 8 [ < e d' b > ] fis 2 ^(  |
%% 45
                    g 4 ) r < g, g > _( < fis fis, > )  |
                    < e, e > 4 _( < cis, cis > ) \stemDown < d, d > 8 ^( [ < e, e > ] < fis, fis > 4 ) \stemNeutral  |
                    < b,, b, > 4 _( < gis,, gis, > ) < a,, a, > 8 _( [ < b,, b, > ] < cis, cis > 4 )  |
                    < d d, > 4 < b, b,, > < cis cis, > < cis f cis, >  |
                    \key a \major
                    < cis' fis a > 1  |
%% 50
                    < fis b d' > 2 ^( < gis e cis' > )  |
                    < e a cis' > 2 ^( < d fis b > )  |
                    < d g b > 2 ^( < e cis a > 4 ) r  |
                    < d a > 4 ^( < a fis > ) < a cis' > ^( < a, cis' > )  |
                    < cis cis' > 8 ^( [ < dis cis' > ] < cis' f > 4 ) < fis cis' > < b b, >  |
%% 55
                    < fis, fis > 4 < a, a > < e e, > < g g, >  |
                    < cis cis, > 4 < e' cis, cis > < cis b f' > 2  |
                    < fis, a cis > 1  |
                    < cis' cis fis fis, > 1  |
                    < a fis > 1  |
%% 60
                    r4 < fis, cis a > < a' fis cis' > 2  |
                    r2 < a fis cis' >  |
                    < fis,, fis, > 1 -\fermata  |
                    \bar "|."
                } % Voice
            >> % Staff (final) ends
        >> % GrandStaff (final) 1

    >> % notes

    \layout {
        indent = 3.0\cm
        short-indent = 1.5\cm
        \context { \Staff \RemoveEmptyStaves }
        \context { \GrandStaff \accepts "Lyrics" }
    }
%     uncomment to enable generating midi file from the lilypond source
%         \midi {
%         } 
} % score
