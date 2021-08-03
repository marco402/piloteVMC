// JSLint options
/*global console:false, document:false, $:false, jQuery:false, setInterval:false, clearInterval:false, Option:false*/
/*jslint indent:4, todo:true, vars:true, unparam:true */

// Modules graphiques
var modHighCharts;
var modJQPlot;
var modFlot;
var modChartJS;
// Fonctions et variables
var start = {}; // = new Date();
var modChart;
var chart_elec0;
var chart_elec1;
var chart_elec2;
var chart_elec3;
var chart_elec4;
//mémorisation locale des data
var chart0_data = {};
var chart1_data = {};
var chart2_data = {};
var chart3_data = {};
var chart4_data = {};
var timerID;
var chart_elec0_delay = 60; // secondes
function enable_timer(func, delay) {
    "use strict";

    timerID = setInterval(func, delay);
}
function disable_timer() {
    "use strict";

    clearInterval(timerID);
}
jQuery(function ($) {
    "use strict";
    modChart = modHighCharts;
  });
function init_chart0_navigation(data) {
    "use strict";

    // Date du calendrier
 //   var curDate = new Date();
 //   curDate.setTime(data.debut);
 //   curDate.setDate(curDate.getDate() - 1); // -1 Jour
 //   $("#chart0_date").val(curDate.getTime());
}
function init_chart1_navigation(data) {
    "use strict";

    // Date du calendrier
  //  var curDate = new Date();
  //  curDate.setTime(data.debut);
  //  curDate.setDate(curDate.getDate() - 1); // -1 Jour
  //  $("#chart1_date").val(curDate.getTime());
}
function init_chart2_navigation(data) {
    "use strict";

    var arrayDuree = [];
    var i;
    var txtdecalage;

    switch (data.periode) {
    case ("jours"):
        for (i = 1; i <= 31; i += 1) {
            arrayDuree[i] = i;
        }
        txtdecalage = "1 jour";
        break;
    case ("semaines"):
        for (i = 1; i <= 52; i += 1) {
            arrayDuree[i] = i;
        }
        txtdecalage = "1 sem.";
        break;
    case ("mois"):
        for (i = 1; i <= 12; i += 1) {
            arrayDuree[i] = i;
        }
        txtdecalage = "1 mois";
        break;
    case ("ans"):
        for (i = 1; i <= 4; i += 1) {
            arrayDuree[i] = i;
        }
        txtdecalage = "1 mois";
        break;
    default:
    }

    // Met à jour la liste déroulante "duree"
    var select = $('.select_chart2#duree');
    var options;
    if (select.prop) {
        options = select.prop('options');
    } else {
        options = select.attr('options');
    }
    $('option', select).remove();
    $.each(arrayDuree, function (val, text) {
        if (val > 0) {
            options[options.length] = new Option(text, val);
        }
    });

    // Valeurs par défaut
    $(".select_chart2#duree").val(data.duree);
    $('.select_chart2#duree').selectmenu('refresh', true);
    //$(".select_chart2#duree").refresh;
    $(".select_chart2#periode").val(data.periode);
    $('.select_chart2#periode').selectmenu('refresh', true);
    //$(".select_chart2#periode").refresh;

    // Libelles des boutons //ui-button-text
    var btn = {};
    /*btn = $("#chart2_date_prec").find('span.ui-button-text');
    if (btn.length === 0) { // jQuery Mobile n'utilise pas de <span>
        btn = btn.prevObject;
    }
    btn.html("- " + txtdecalage);*/
    btn = $("#chart2_date_prec");
    if ($.mobile) {
        btn.html("- " + txtdecalage);
    } else {
        btn.button("option", "label", "- " + txtdecalage);
    }

    /*btn = $("#chart2_date_suiv").find('span.ui-button-text');
    if (btn.length === 0) { // jQuery Mobile n'utilise pas de <span>
        btn = btn.prevObject;
    }
    btn.html("+ " + txtdecalage);*/
    btn = $("#chart2_date_suiv");
    if ($.mobile) {
        btn.html("+ " + txtdecalage);
    } else {
        btn.button("option", "label", "+ " + txtdecalage);
    }

    // Date du calendrier
    var curDate = new Date();
    curDate.setTime(data.debut);
    curDate.setDate(curDate.getDate() - 1); // -1 Jour
    $("#chart2_date").val(curDate.getTime());
}
function init_chart3_navigation(data) {
    "use strict";

    // Date du calendrier
   // var curDate = new Date();
   // curDate.setTime(data.debut);
   // curDate.setDate(curDate.getDate() - 1); // -1 Jour
   // $("#chart3_date").val(curDate.getTime());
}
function init_chart4_navigation(data) {
    "use strict";

    // Date du calendrier
   // var curDate = new Date();
   // curDate.setTime(data.debut);
   // curDate.setDate(curDate.getDate() - 1); // -1 Jour
   // $("#chart4_date").val(curDate.getTime());
}
/*
function chart_loaded(targetId, subtitle, chart_chrono) {
    "use strict";

    if (start > 0) {
        var stChrono = '<em>Construit en ' + (new Date() - start) + 'ms</em>';
        if (chart_chrono !==  undefined) {
            // Cas où un a un callback vers une fonction spécifique (HighCharts par exemple)
            chart_chrono(targetId, stChrono);
        } else {
            // Cas où on n'a pas de callback
            if ($(targetId + '_chrono').length === 0) {
                $(targetId).before('<div class="chart_chrono" id="' + targetId.substring(1) + '_chrono"></div>');
            }
            $(targetId + '_chrono').html(stChrono);
        }

       // if ($(targetId + '_legende').length) {
       //     if (subtitle.length > 0) { $(targetId + '_legende').show(); }
       //     $(targetId + '_legende').html(subtitle);
       // }

        start = 0;
    }
}
*/
function tooltip_chart0(thisSerieNum, thisPtX) {
    "use strict";

    var tooltip;
    var tipDate = new Date(chart0_data.debut).toLocaleString();
    var tipName = chart0_data.series[Object.keys(chart0_data.series)[thisSerieNum]];

    tooltip = 'Le ' + tipDate + '<br />';
    tooltip += '<b>' + chart0_data.data[Object.keys(chart0_data.series)[thisSerieNum]].toLocaleString() + ' ' + tipName + '</b><br />';

    return tooltip;
}
function tooltip_chart134(thisSerieNum) {
    "use strict";

    var tooltip;
    var tipDate;
var passe=false;
    
    $.each(thisSerieNum.points, function (index,courbe) {
 
    
            if (passe==false)
            {
               // pointFormat = "Value: {point.y:.2f}";
               //valueDecimals= 2,
                tipDate = new Date(courbe.key).toLocaleString();
                tooltip ='<span style= "font-size: 0.85em;">' + "Le " + tipDate +"(" + courbe.x/1000 +")</span>  </b><br/>";
                passe=true
            }
            tooltip +='<span  style="color:' + courbe.color + '; font-size: 0.85em;">'  + courbe.series.name + ':';
            if(courbe.series.name.indexOf('mode')!=-1)
            {
                if(courbe.y==0)
                    tooltip +='arrêt';
                else if(courbe.y==1)
                    tooltip +='lent';
                else if(courbe.y==2)
                    tooltip +='rapide';
                else if(courbe.y==3)
                    tooltip +='auto';
                else if(courbe.y==4)
                    tooltip +='forçé lent';
                else if(courbe.y==5)
                    tooltip +='forçé rapide';
                else if(courbe.y==6)
                    tooltip +='forçé arrêt';
                else if(courbe.y==7)
                    tooltip +='été';
                else if(courbe.y==8)
                    tooltip +='hiver';
                else
                    tooltip +='cas inex';
            }
            else if(courbe.series.name.indexOf('marche')!=-1)
            {
                if(courbe.y==0)
                    tooltip +='arrêt';
                else
                    tooltip +='marche';
            }
            else if(courbe.series.name.indexOf('rapide') !=-1)
            {
                if(courbe.y==0)
                    tooltip +='lent';
                else
                    tooltip +='rapide';
            }
            else 
                tooltip += Number.parseFloat(courbe.y).toFixed(2) ;

            tooltip +=' </span> </b><br/>';
            //tooltip +='<span  style="color:' + courbe.color + '; font-size: 0.85em;">'  + courbe.series.name + ':'+ Number.parseFloat(courbe.y).toFixed(2) + ' </span> </b><br/>';
      });
 
 
    return tooltip;
}
function tooltip_chart2(leChart2) {
    "use strict";

    var tooltip = "";
    var tipDate;
    var tipName;
    var tipTotal;
    var tipPrix;
    var tipDetail;


//var thisSerieNum=leChart2.series.index;
//$thisPtX= leChart2.point.x;
    //if (leChart2.series.index < Object.keys(chart2_data.series).length) { // Période courante
        tipName = Object.keys(chart2_data.series)[leChart2.series.index];
        tipDate = chart2_data.categories[leChart2.point.x];
        tipTotal = 0;
        $.each(chart2_data.series, function (serie_name, serie_title) {
            tipTotal += chart2_data[serie_name + "_data"][leChart2.point.x];
        });
        tipPrix = chart2_data.prix;
        tipDetail = chart2_data;
   // } else { // Période Précédente
   //     tipName = 'PREC';
   //     tipDate = chart2_data.PREC_data[leChart2.point.x] === null ? null : chart2_data.PREC_data[leChart2.point.x][0];
   //     tipTotal = chart2_data.PREC_data[leChart2.point.x] === null ? 0 : chart2_data.PREC_data[leChart2.point.x][1];
   //     tipPrix = chart2_data.PREC_prix;
   //     tipDetail = chart2_data.PREC_detail;
   // }

    // Date & Consommation
    if (tipDate !== null) {
        tooltip = tipDate + '<br />';
        tooltip += '<span><b>' + chart2_data.optarif[Object.keys(chart2_data.optarif)[0]] + ' </b></span><br />';
        tooltip += '<span style="color:' + chart2_data[tipName + "_color"] + '"><b>' + chart2_data[tipName + "_name"] + '</b></span><br />';

        // Abonnement
        tooltip += 'Abonnement : ' + tipPrix.ABONNEMENTS[leChart2.point.x].toFixed(2) + ' &euro;<br />';

        // Taxes
        tooltip += 'Taxes :<br />';
        $.each(tipPrix.TAXES, function (serie_name, serie_data) {
            tooltip += serie_name + ' : ' + tipPrix.TAXES[serie_name][leChart2.point.x].toFixed(2) + ' &euro;<br />';
        });

        // Coût détaillé
        tooltip += 'Consommé :<br />';
        $.each(chart2_data.series, function (serie_name, serie_title) {
            if (tipPrix.TARIFS[serie_name][leChart2.point.x] !== 0) {
                tooltip += '<span style="color:' + chart2_data[serie_name + "_color"] + '">';
                if ((serie_name === tipName) && (Object.keys(chart2_data.series).length > 1)) {
                    tooltip += "<b>&#9758;&nbsp;</b>";
                }
                tooltip += serie_name + ' : ' + tipPrix.TARIFS[serie_name][leChart2.point.x].toFixed(2) + ' &euro;';
                tooltip += ' (' + tipDetail[serie_name + "_data"][leChart2.point.x].toFixed(1) + ' kWh)<br />';
                tooltip += '</span>';
            }
        });

        // Coût total
        tooltip += '<b>Total : ' + tipPrix.TOTAL[leChart2.point.x].toFixed(2) + ' &euro;<b>';
        tooltip += '<b> (' + tipTotal.toFixed(1) + ' kWh)</b><br />';
        
        //marc ajout annotations
        if(tipDetail["ANNOTATIONS"][leChart2.point.x]!==null)
                 tooltip += '<span style="color:red">' + tipDetail["ANNOTATIONS"][leChart2.point.x];       
        }
    return tooltip;
}
function refresh_chart0(date) {
    "use strict";

    // Seulement si le graphique est visible
    
    if ($('#chart0').is(":visible")) {
        // Remise à zéro du chronomètre
        start = new Date();

        // Désactivation du rafraichissement automatique (le cas échéant)
        disable_timer();

        // Lancement de la requête instantly
        var parameters = "";
        $.getJSON('json.php?query=instantly' + parameters, function (data) {
            // Création / Remplacement du graphique
            if (chart_elec0) {
                if (Array.isArray(chart_elec0)) {
                    // Chaque gauge est un graphique séparé
                    $.each(chart_elec0, function (ch0_key, ch0_val) {
                        ch0_val.destroy();
                    });
                } else {
                    // Chaque gauge est un élément du même graphique
                    chart_elec0.destroy();
                }
            }
            // Keep some data
            chart0_data = data;
            chart_elec0 = modChart.init_chart0(data);
            init_chart0_navigation(data);

            // Activation du rafraichissement automatique
            chart_elec0_delay = data.refresh_delay;
            if (data.refresh_auto) {
                enable_timer(refresh_chart0, chart_elec0_delay * 1000);
            }
        });
    }
}
function setAnnotationToDatabase(annot,t)
{
    "use strict";
  var parameters = ("&annot=" + annot + "&t=" + t );
  $.getJSON('json.php?query=setannot' + parameters, function (data) { 
    if(data)
    {
        var curdate = chart1_data.debut;
        var newdate = new Date();
        newdate.setTime(curdate);
        refresh_chart1(newdate);
    }  
  } ); 
    // alert('pb enregistrement annotation: ' + annot);
}
function refresh_chart1(date) {
    "use strict";

    // Seulement si le graphique est visible
    if ($('#chart1').is(":visible")) {
        // Remise à zéro du chronomètre
        start = new Date();

        // Lancement de la requête daily
        var parameters = (date ? "&date=" + date.getTime() / 1000 : "");
        $.getJSON('json.php?query=daily' + parameters, function (data) {
            // Création / Remplacement du graphique
            if (chart_elec1) {
                chart_elec1.destroy();
            }
            // Keep some data
            chart1_data = data;
            chart_elec1 = modChart.init_chart1(data);
            init_chart1_navigation(data);
        });
    }
}
function refresh_chart2(duree, periode, date) {
    "use strict";

    // Seulement si le graphique est visible
    if ($('#chart2').is(":visible")) {
        // Remise à zéro du chronomètre
        start = new Date();

        // Lancement de la requête historique
        var parameters = (duree ? "&duree=" + duree : "") + (periode ? "&periode=" + periode : "") + (date ? "&date=" + date.getTime() / 1000 : "");
        $.getJSON('json.php?query=history' + parameters, function (data) {
            // Création / Remplacement du graphique
            if (chart_elec2) {
                chart_elec2.destroy();
            }
            // Keep some data
            chart2_data = data;
            chart_elec2 = modChart.init_chart2(data);
            init_chart2_navigation(data);
        });
    }
}
function refresh_chart3(date) {
    "use strict";

    // Seulement si le graphique est visible
    if ($('#chart3').is(":visible")) {
        // Remise à zéro du chronomètre
        start = new Date();

        // Lancement de la requête daily
        var parameters = (date ? "&date=" + date.getTime() / 1000 : "");
        $.getJSON('json.php?query=get_vmc' + parameters, function (data) {
            // Création / Remplacement du graphique
            if (chart_elec3) {
                chart_elec3.destroy();
            }
            // Keep some data
            chart3_data = data;
            chart_elec3 = modChart.init_chart3(data);
            init_chart3_navigation(data);
        });
    }
}
function refresh_chart4(date) {
    "use strict";

    // Seulement si le graphique est visible
    if ($('#chart4').is(":visible")) {
        // Remise à zéro du chronomètre
        start = new Date();

        // Lancement de la requête daily
        var parameters = (date ? "&date=" + date.getTime() / 1000 : "");
        $.getJSON('json.php?query=get_fonctionnel' + parameters, function (data) {
            // Création / Remplacement du graphique
            if (chart_elec4) {
                chart_elec4.destroy();
            }
            // Keep some data
            chart4_data = data;
            chart_elec4 = modChart.init_chart4(data);
            init_chart4_navigation(data);
        });
    }
}
function process_chart0_button(object) {
    "use strict";

    //var optarif = $("#chart0").highcharts().optarif;
    var optarif = chart0_data.optarif;

    refresh_chart0(optarif, null);
}
function process_chart1_button(object) {
    "use strict";

    //var curdate = $("#chart1").highcharts().debut;
    var curdate = chart1_data.debut;

    var newdate = new Date();
    newdate.setTime(curdate);

    switch (object.value) {
    case "date":
        newdate = undefined;
        $("#chart1_date").datepicker('show');
        break;
    case "1prec":
        newdate.setDate(newdate.getDate() - 1);
        break;
    case "1suiv":
        newdate.setDate(newdate.getDate() + 1);
        break;
    case "now":
        newdate = null;
        break;
    default:
        newdate = null;
    }

    if (newdate !== undefined) {
        refresh_chart1(newdate);
    }
}
function process_chart2_button(object) {
    "use strict";

    var periode = $(".select_chart2#periode").val();
    var duree = $(".select_chart2#duree").val();
    //var curdate = $("#chart2").highcharts().debut;
    var curdate = chart2_data.debut;
    var newdate = new Date();
    newdate.setTime(curdate);

    var coefdate;
    // Type de changement de date
    switch (object.value) {
    case "date":
        newdate = undefined;
        $("#chart2_date").datepicker('show');
        break;
    case "1prec":
        // on recule
        coefdate = -1;
        break;
    case "1suiv":
        // on avance
        coefdate = 1;
        break;
    case "now":
        // retour à aujourd'hui
        newdate = null;
        break;
    default:
        // on ne change rien
        coefdate = 0;
    }

    // Calcul du décalage de date
    if (newdate && (coefdate !== 0)) {
        switch (periode) {
        case ("jours"):
            // décalage d'un jour
            newdate.setDate(newdate.getDate() + coefdate);
            break;
        case ("semaines"):
            // décalage d'une semaine
            newdate.setDate(newdate.getDate() + 7 * coefdate);
            break;
        case ("mois"):
            // décalage d'un mois
            newdate.setMonth(newdate.getMonth() + coefdate);
            break;
        case ("ans"):
            // décalage d'un mois
            newdate.setMonth(newdate.getMonth() + coefdate);
            break;
        default:
            // on ne change rien
            newdate.setDate(newdate.getDate());
        }
    }

    if (newdate !== undefined) {
        refresh_chart2(duree, periode, newdate);
    }
}
var lesSeries=[true,true,true,true,true,true,true,true];
function process_chart3_button(object) {
    "use strict";
    //var curdate = $("#chart3").highcharts().debut;
    var curdate = chart3_data.debut;

    var newdate = new Date();
    newdate.setTime(curdate);

    switch (object.value) {
    case "date":
        newdate = undefined;
        $("#chart3_date").datepicker('show');
        return;
    case "1prec":
        newdate.setDate(newdate.getDate() - 1);
        refresh_chart3(newdate);
        lesSeries=[true,true,true,true,true,true,true,true];
        return;
    case "1suiv":
        newdate.setDate(newdate.getDate() + 1);
        refresh_chart3(newdate);
        lesSeries=[true,true,true,true,true,true,true,true];
        return;
    //case "now":  le bouton ToDay met juste le jour courant en surbrillance
    //    newdate = null;
    //    refresh_chart3(newdate);
    //    return;
    //default:
       // newdate = null;
    }
    if (newdate !== undefined) {
         //refresh_chart3(newdate);
        //var $lesSeries;
        //newdate.setDate(curdate);
        switch (object.value) {
            case "HumCuis":
                   // $lesSeries=[true,false,false,false,false,false,false,false];
                   lesSeries[0]=!lesSeries[0];
                break; 
            case "HumSdb":
                    lesSeries[1]=!lesSeries[1];
                   //  $lesSeries=[false,true,false,false,false,false,false,false];
                break;               
            case "TempExt":
                    lesSeries[2]=!lesSeries[2];
                   //  $lesSeries=[false,false,true,false,false,false,false,false];
                break;               
            case "TempCuis":
                    lesSeries[3]=!lesSeries[3];
                  //   $lesSeries=[false,false,false,true,false,false,false,false];
                break;               
            case "TempSdb":
                    lesSeries[4]=!lesSeries[4];
                   //  $lesSeries=[false,false,false,false,true,false,false,false];
                break;               
            case "Mode":
                    lesSeries[5]=!lesSeries[5];
                   //  $lesSeries=[false,false,false,false,false,true,false,false];
                break;               
            case "MarAre":
                    lesSeries[6]=!lesSeries[6];
                   //  $lesSeries=[false,false,false,false,false,false,true,false];
                break;               
            case "Vitesse":
                    lesSeries[7]=!lesSeries[7];
                   //  $lesSeries=[false,false,false,false,false,false,false,true];
                 break;
           case "Tout":
                  lesSeries=[true,true,true,true,true,true,true,true];
                 break;                                             
            }
            //refresh_chart3(newdate);
            
        }
   afficheChart3(chart_elec3,lesSeries);      
 }
function process_chart4_button(object) {
    "use strict";

    //var curdate = $("#chart4").highcharts().debut;
    var curdate = chart4_data.debut;

    var newdate = new Date();
    newdate.setTime(curdate);

    switch (object.value) {
    case "date":
        newdate = undefined;
        $("#chart4_date").datepicker('show');
        break;
    case "1prec":
        newdate.setDate(newdate.getDate() - 1);
        break;
    case "1suiv":
        newdate.setDate(newdate.getDate() + 1);
        break;
    case "now":
        newdate = null;
        break;
    default:
        newdate = null;
    }

    if (newdate !== undefined) {
        refresh_chart4(newdate);
    }
}
function refresh_charts(pageName) {
    "use strict";

    switch (pageName) {
    case 'page0':
        // Crée le graphique 0 (instantly)
        refresh_chart0();
        break;
    case 'page1':
        // Crée le graphique 1 (daily)
        refresh_chart1();
        break;
    case 'page2':
        // Crée le graphique 2 (history)
        refresh_chart2();
        break;
    case 'page3':
        // Crée le graphique 3 (history)
        refresh_chart3();
        break;
    case 'page4':
        // Crée le graphique 4 (history)
        refresh_chart4();
        break;
    default:
        // Crée le graphique 0 (instantly)
        refresh_chart0();
        // Crée le graphique 1 (daily)
        refresh_chart1();
        // Crée le graphique 2 (history)
        refresh_chart2();
        // Crée le graphique 3 (history)
        refresh_chart3();
        // Crée le graphique 4 (history)
        refresh_chart4();

    }
}
function change_date() {
    "use strict";
    var thisID = $(this).attr("id");

    var newdate = new Date();
    newdate.setTime($(this).val());
    newdate.setDate(newdate.getDate() + 1); // +1 Jour

    switch (thisID) {
    case 'chart0_date':
        refresh_chart0(newdate);
        break;
    case 'chart1_date':
        refresh_chart1(newdate);
        break;
    case 'chart2_date':
        refresh_chart2(newdate);
        break;
    case 'chart3_date':
        refresh_chart3(newdate);
        break;
    case 'chart4_date':
        refresh_chart4(newdate);
        break;
    default:
    }
}
function init_events() {
    "use strict";

    // Evénements boutons (click)
    $('.button_chart0').off('click').click(
        function () {process_chart0_button(this); }
    );
    $('.button_chart1').off('click').click(
        function () {process_chart1_button(this); }
    );
    $('.button_chart2').off('click').click(
        function () {process_chart2_button(this); }
    );
    $('.button_chart3').off('click').click(
        function () {process_chart3_button(this); }
    );
    $('.button_chart4').off('click').click(
        function () {process_chart4_button(this); }
    );
    // Evénement selectmenu (change)
    if ($.mobile) {
        $('.select_chart2').off('change').change(
            function () {process_chart2_button(this); }
        );
    } else {
        $('.select_chart2').selectmenu({
            change: function () {process_chart2_button(this); }
        });
    }
    // Evénement datepicker (select)
    $('.datepicker').each(function () {
        var itemID = "#" + $(this).attr('id');
        $(itemID).datepicker("option", "onSelect", change_date);
    });
}
function afficheChart3(chart,lesSeries)
{
        for(i=0;i<8;i++)
            chart.series[i].setVisible(lesSeries[i],false);//on affiche que les true
    refresh(chart);
}
//ATTENTION au piege,il faut que la hauteur soit suffisante pour le tooltip shared true sinon affiche que les points sans tooltip
//ATTENTION au piege,lorsque la larger est trop grande et qu'on affiche 8 ou 14 mois en historique,parfois des couleurs s'ajoutent aux dernières colonnes
//ok avec edge et chrome si lancé par visual studio code , pb avec IE et chrome lancé directement ??????
function refresh(chart)
{ 
    chart.redraw();	
    chart.reflow();
}
if ($.mobile) {
        //jq mobile loaded
        $(document).on("pageshow", '[data-role="page"]', function (event, ui) {
        "use strict";

        // Initialisation jQueryUI datepicker
        $.datepicker.setDefaults($.datepicker.regional.fr);
        $('.datepicker').each(function () {
            var itemID = "#" + $(this).attr('id');
            $(itemID).datepicker({
                showAnim: "blind",
                showOn: "button",
                //buttonImage: "images/glyphish/83-calendar.png",
                //buttonImage: "images/tango icons/X-office-calendar-alpha.png",
                buttonText: "Sélection de la date",
                buttonImageOnly: true,
                showButtonPanel: true,
                changeMonth: true,
                changeYear: true,
                showOtherMonths: true,
                selectOtherMonths: true,
                dateFormat: $.datepicker.TIMESTAMP, // $.datepicker.ISO_8601,
                maxDate: 0
            });
        });

        // Enhance tooltip appearence, using JQuery styling
        if ($(document).tooltip) {
            // UI loaded
            $(document).tooltip({
                // use the built-in fadeIn/fadeOut effect
                effect: "fade"
            });
        }

        init_events();

        var pageName;
        // Disable previous indicator
        if (typeof (ui.prevPage) === 'object') {
            pageName = ui.prevPage.attr("id");
            $("#nav_" + pageName, "#" + pageName).removeClass("ui-btn-active");
        }
        // Enable selected indicator
        if (typeof ($(this)) === 'object') {
            pageName = $(this).attr("id");
            $("#nav_" + pageName, "#" + pageName).addClass("ui-btn-active");
        }

    });
    $(document).on("pagechange", function (event, ui) {
        "use strict";

        var pageName;
        if (typeof (ui.toPage) === 'object') {
            pageName = ui.toPage.attr("id");
            refresh_charts(pageName);
        }
    });

    // Sablier durant les requêtes AJAX (style CSS à définir)
    $(document)
        .ajaxStart(function () {
            "use strict";

            $.mobile.loading("show");
            // Désactive les éléments de navigation
            $('.ui-page').addClass("ui-state-disabled");
            //$('.ui-btn').addClass("ui-state-disabled");
            //$('.ui-select').addClass("ui-state-disabled");
        })
        .ajaxStop(function () {
            "use strict";

            // Supprime la classe CCS 'busy'
            $.mobile.loading("hide");
            // Active les éléments de navigation
            $('.ui-page').removeClass("ui-state-disabled");
            //$('.ui-btn').removeClass("ui-state-disabled");
            //$('.ui-select').removeClass("ui-state-disabled");
        });

} else {
    $(document).ready(function () {
        "use strict";

        // Initialisation jQueryUI button
        $('.button_chart0').button();
        $('.button_chart1').button();
        $('.button_chart2').button();
        $('.button_chart3').button();
        $('.button_chart4').button();
        // Icones jQueryUI / https://jqueryui.com/upgrade-guide/1.12/#button
        $('#chart0_refresh').button({icon: "ui-icon-refresh", iconPosition: "beginning"});

        $('#chart1_date_prec').button({icon: "ui-icon-arrowthick-1-w", iconPosition: "beginning"});
        $('#chart1_date_select').button({icon: "ui-icon-calendar", iconPosition: "beginning"})
            .append('<span class="ui-icon ui-icon-triangle-1-s">');
        $('#chart1_date_suiv').button({icon: "ui-icon-arrowthick-1-e", iconPosition: "end"});

        $('#chart2_date_prec').button({icon: "ui-icon-arrowthick-1-w", iconPosition: "beginning"});
        $('#chart2_date_now').button({icon: "ui-icon-calendar", iconPosition: "beginning"});
        $('#chart2_date_select').button({icon: "ui-icon-calendar", iconPosition: "beginning"})
            .append('<span class="ui-icon ui-icon-triangle-1-s">');
        $('#chart2_date_suiv').button({icon: "ui-icon-arrowthick-1-e", iconPosition: "end"});

        // Initialisation jQueryUI selectmenu
        $('.select_chart2').selectmenu({
            dropdown: false
        });
        // Overflow : permet de limiter la hauteur des listes déroulantes (via css)
        $('.select_chart2').selectmenu("menuWidget").addClass("ui-selectmenu-overflow");

        $('#chart3_date_prec').button({icon: "ui-icon-arrowthick-1-w", iconPosition: "beginning"});
        $('#chart3_date_now').button({icon: "ui-icon-calendar", iconPosition: "beginning"});
        $('#chart3_date_select').button({icon: "ui-icon-calendar", iconPosition: "beginning"})
            .append('<span class="ui-icon ui-icon-triangle-1-s">');
        $('#chart3_date_suiv').button({icon: "ui-icon-arrowthick-1-e", iconPosition: "end"});

        
        $('#chart4_date_prec').button({icon: "ui-icon-arrowthick-1-w", iconPosition: "beginning"});
        $('#chart4_date_now').button({icon: "ui-icon-calendar", iconPosition: "beginning"});
        $('#chart4_date_select').button({icon: "ui-icon-calendar", iconPosition: "beginning"})
            .append('<span class="ui-icon ui-icon-triangle-1-s">');
        $('#chart4_date_suiv').button({icon: "ui-icon-arrowthick-1-e", iconPosition: "end"});

        // Initialisation jQueryUI datepicker
        $.datepicker.setDefaults($.datepicker.regional.fr);
        $('.datepicker').each(function () {
            var itemID = "#" + $(this).attr('id');
            $(itemID).datepicker({
                showAnim: "blind",
                showOn: "button",
                //buttonImage: "images/glyphish/83-calendar.png",
                //buttonImage: "images/tango icons/X-office-calendar-alpha.png",
                buttonText: "Sélection de la date",
                buttonImageOnly: true,
                showButtonPanel: true,
                changeMonth: true,
                changeYear: true,
                showOtherMonths: true,
                selectOtherMonths: true,
                dateFormat: $.datepicker.TIMESTAMP, // $.datepicker.ISO_8601,
                maxDate: 0
            });
        });

        // Enhance tooltip appearence, using JQuery styling
        if ($(document).tooltip) {
            // UI loaded
            $(document).tooltip({
                // use the built-in fadeIn/fadeOut effect
                effect: "fade"
            });
        }

        init_events();

        // Enable tab navigation
        if ($('#tabs').length > 0) {
            $('#tabs')
                .tabs({
                    create: function (event, ui) {
                        var pageName;
                        if (typeof (ui.panel) === 'object') {
                            pageName = ui.panel.attr("id");
                            refresh_charts(pageName);
                        }
                    },
                    activate: function (event, ui) {
                        var pageName;
                        if (typeof (ui.newPanel) === 'object') {
                            pageName = ui.newPanel.attr("id");
                            refresh_charts(pageName);
                        }
                    }
                });
        } else {
            // Rafraîchit tous les graphiques à la fois
            refresh_charts();
        }

        // Sablier durant les requêtes AJAX (style CSS à définir)
        $(document)
            .ajaxStart(function () {
                //$('.wait').show();
                $('.wait').addClass("ui-icon-loading");
                // Désactive les éléments de navigation
                $('.ui-tabs-panel').addClass("ui-state-disabled");
                //$('.ui-button').addClass("ui-state-disabled");
                //$('.ui-selectmenu-button').addClass("ui-state-disabled");
            })
            .ajaxStop(function () {
                //$('.wait').hide();
                $('.wait').removeClass("ui-icon-loading");
                // Active les éléments de navigation
                $('.ui-tabs-panel').removeClass("ui-state-disabled");
                //$('.ui-button').removeClass("ui-state-disabled");
                //$('.ui-selectmenu-button').removeClass("ui-state-disabled");
            });
    });
}
