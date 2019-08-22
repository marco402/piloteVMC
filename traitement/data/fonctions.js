// Le fichier d'origine fait partie de l'application ESP8266 Teleinfo WEB Server
// https://github.com/hallard/LibTeleinfo
	 // <!-- Scripts devant être chargés en fin de page -->
			var Timer_sys;
			var Timer_vmc;			
			var Timer_tinfo;  
			var counters={};
			var isousc, iinst;
			var elapsed = 0;
			function formatSize(bytes) {
			  if (bytes < 1024)  return bytes+' Bytes';
			  if (bytes < (1024*1024)) return (bytes/1024).toFixed(0)+' KB';
			  if (bytes < (1024*1024*1024)) return (bytes/1024/1024).toFixed(0)+' MB';
			  return (bytes/1024/1024/1024).toFixed(0)+' GB';
		  }
			function rowStyle(row, index) {  
				var flags=parseInt(row.fl,10);  
				if (flags & 0x80) return {classes:'danger'};
				if (flags & 0x02) return {classes:'warning'};
				if (flags & 0x08) return {classes:'success'};
				return {}; 
			} 
			function labelFormatter(value, row) {  
				var flags=parseInt(row.fl,10);  
				
				if ( typeof counters[value]==='undefined') 
					counters[value]=1;  
				if ( flags & 0x88 ) 
					counters[value]++;  
				return value + ' <span class=\"badge\">'+counters[value]+'</span>';   
				}  
			function valueFormatter(value, row) {  
				if (row.na=="ISOUSC")  
					isousc=parseInt(value);  
				else if (row.na=="IINST") {  
					var pb, pe, cl;  
					iinst=parseInt(value);  
					pe=parseInt(iinst*100/isousc);  
					if (isNaN(pe)) 
						pe=0;  
					cl='success';  
					if (pe>70) cl ='info';  
					if (pe>80) cl ='warning';  
					if (pe>90) cl ='danger';  

					cl = 'progress-bar-' + cl;  
					if (pe>0) 
						$('#scharge').text(pe+'%');  
					if (typeof isousc!='undefined')  
						$('#pcharge').text(iinst+'A / '+isousc+'A');  
					$('#pbar').attr('class','progress-bar '+cl);  
					$('#pbar').css('width', pe+'%');
				}   
				return value;         
			}  
			function fileFormatter(value, row) {  
				var fname = row.na;
				var htm = '';  
				htm+= '<a href="' + fname + '">' + fname + '</a>';
				return htm;
			}  
			function RSSIFormatter(value, row) {  
				var rssi=parseInt(row.rssi);  
				var signal = Math.min(Math.max(2*(rssi+100),0),100);
				var cl, htm;  
				cl='success';  
				if (signal<70) cl ='info';  
				if (signal<50) cl ='warning';  
				if (signal<30) cl ='danger';  
				cl = 'progress-bar-' + cl;  
				htm = "<div class='progress progress-tbl'>";
				htm+= "<div class='progress-bar "+cl+"' role='progressbar' aria-valuemin='0' aria-valuemax='100' ";
				htm+= "aria-valuenow='"+signal+"' style='width:"+signal+"%'>"+rssi+"dB</div></div>";
				return htm;
			}  
			function Notify(mydelay, myicon, mytype, mytitle, mymsg) {  
				$.notify(
					{	icon:'glyphicon glyphicon-'+myicon,
						title:'&nbsp;<strong>'+mytitle+'</strong>',
						message:'<p>'+mymsg+'</p>',
					},{
						type:mytype,
						//showProgressbar: true,
						animate:{enter:'animated fadeInDown',exit:'animated fadeOutUp',delay:mydelay*1000}
					}
				);
			}  
			function progressUpload(data) {
			  if(data.lengthComputable) {
			  	var pe = (data.loaded/data.total*100).toFixed(0) ;
			  	$('#pfw').css('width', pe +'%');
			   	$('#psfw').text(formatSize(data.loaded)+' / '+formatSize(data.total));  
			  }
			}
			//Upload file facility -->
			function progressUploadf(data) {
			  if(data.lengthComputable) {
			  	var pef = (data.loaded/data.total*100).toFixed(0) ;
			  	$('#pfile').css('width', pef +'%');
			   	$('#psfile').text(formatSize(data.loaded)+' / '+formatSize(data.total));  
			  }
			}
			function waitReboot() {
   			var url = location.protocol+'//'+location.hostname+(location.port ? ':'+location.port: '') + '/#tab_sys' ;
				$('#txt_srv').text('Tentative de connexion à '+url);
				$('#mdl_wait').modal();
				var thistimer=setInterval(function() {
			    $.ajax({
			      cache: false,
			      type: 'GET',
			      url: '/hb.htm',
			      timeout: 900,
			      success: function(data, textStatus, XMLHttpRequest) {
			      	console.log(data);
			        if (data === 'OK') {
			  				$('#mdl_wait').modal('hide');	
			  				clearInterval(thistimer);
			  				window.location = url;
			  				location.reload();
		  					elapsed=0;
			        }
			      }
			    });
					elapsed++;
					$('#txt_elapsed').text('Temps écoulé ' + elapsed + ' s');
				}
				,1000);  
			}
			$('a[data-toggle=\"tab\"]').on('shown.bs.tab', function (e) {  
				clearTimeout(Timer_sys); 
				clearTimeout(Timer_vmc); 				
				clearTimeout(Timer_tinfo); 
				var target = $(e.target).attr("href")  
				console.log('activated ' + target );  

		    // IE10, Firefox, Chrome, etc.
				if (history.pushState) 
			        window.history.pushState(null, null, target);
				else 
			        window.location.hash = target;
				if (target=='#tab_tinfo')  {
					$('#tab_tinfo_data').bootstrapTable('refresh',{silent:true, url:'/tinfo.json'}); 
				} else if (target=='#tab_sys') {
					$('#tab_sys_data').bootstrapTable('refresh',{silent:true, url:'/system.json'});  
				} else if (target=='#tab_vmc') {
					$('#tab_vmc_data').bootstrapTable('refresh',{silent:true, url:'/vmc.json'});  
				} else if (target=='#tab_fs') {
					$.getJSON( "/spiffs.json", function(spiffs_data) { 
						var pb, pe, cl;  		
						total= spiffs_data.spiffs[0].Total; 
						used = spiffs_data.spiffs[0].Used; 
						freeram = spiffs_data.spiffs[0].Ram; 
						$('#tab_fs_data').bootstrapTable('load', spiffs_data.files, {silent:true, showLoading:true});  
						pe=parseInt(used*100/total);  
						if (isNaN(pe)) 
							pe=0;  
						cl='success';  
						if (pe>70) cl ='warning';  
						if (pe>85) cl ='danger';  

						cl = 'progress-bar-' + cl;  
						if (pe>0) 
							$('#sspiffs').text(pe+'%');  
						$('#fs_use').text(formatSize(used)+' / '+formatSize(total));  
						$('#pfsbar').attr('class','progress-bar '+cl);  
						$('#pfsbar').css('width', pe+'%');
					})
					.fail(function() { console.log( "error while requestiong spiffs data" );	})
				} else if (target=='#tab_cfg') {
					$.getJSON( "/config.json", function(form_data) { 
						$("#frm_config").autofill(form_data); 
						})
						.fail(function() { console.log( "error while requestiong configuration data" );	})

					$('#tab_scan_data').bootstrapTable('refresh',{silent:true, showLoading:true, url:'/wifiscan.json'});  
				}
			});  
			$('#tab_tinfo_data').on('load-success.bs.table', function (e, data) {  
				console.log('#tab_tinfo_data loaded');  
		 		if ($('.nav-tabs .active > a').attr('href')=='#tab_tinfo')  
		  		Timer_tinfo=setTimeout(function(){$('#tab_tinfo_data').bootstrapTable('refresh',{silent: true})},2000);  
			}) 
			$('#tab_sys_data').on('load-success.bs.table', function (e, data) {  
				console.log('#tab_sys_data loaded');  
		 		if ($('.nav-tabs .active > a').attr('href')=='#tab_sys')  
		  		Timer_sys=setTimeout(function(){$('#tab_sys_data').bootstrapTable('refresh',{silent: true})},1000);  
			})
			$('#tab_vmc_data').on('load-success.bs.table', function (e, data) {  
				console.log('#tab_vmc_data loaded');  
		 		if ($('.nav-tabs .active > a').attr('href')=='#tab_vmc')  
		  		Timer_vmc=setTimeout(function(){$('#tab_vmc_data').bootstrapTable('refresh',{silent: true})},1000);  
			})
			$('#tab_fs_data').on('load-success.bs.table', function (e, data) {  
				console.log('#tab_fs_data loaded');  
			})
			.on('load-error.bs.table', function (e, status) {  
				console.log('Event: load-error.bs.table');  
					// myTimer=setInterval(function(){myRefresh()},5000);  
			});  
			$('#tab_scan').on('click-row.bs.table', function (e, name, args) {  
				var $form = $('#tab_cfg');  
				$('#ssid').val(name.ssid);  
				setTimeout(function(){$('#psk').focus()},500);  
				$('#tab_scan').modal('hide');  
			});  
			$('#btn_scan').click(function () {  
				$('#tab_scan_data').bootstrapTable('refresh',{url:'/wifiscan.json',showLoading:false,silent:true});  
  		});  
			$('#btn_reset').click(function () {  
				$.post('/factory_reset');  
			  waitReboot(); 
			  return false;
			});  
			$('#btn_reboot').click(function () {  
			  $.post('/reset'); 
			  waitReboot(); 
			  return false;
			});  
			$(document).ready(function(){
            //	$('#tab_info').tab('show');
				$('#tab_sys').tab('show');
			});
			$(document)
				.on('change', '.btn-file :file', function() {
				  var input = $(this),
	      	numFiles = input.get(0).files ? input.get(0).files.length : 1,
	      	label = input.val().replace(/\\/g, '/').replace(/.*\//, '');
	  			input.trigger('fileselect', [numFiles, label]);
				})
		    .on('show.bs.collapse', '.panel-collapse', function () {
		        var $span = $(this).parents('.panel').find('span.pull-right.glyphicon');
		        $span.removeClass('glyphicon-chevron-down').addClass('glyphicon-chevron-up');
		    })
		    .on('hide.bs.collapse', '.panel-collapse', function () {
		        var $span = $(this).parents('.panel').find('span.pull-right.glyphicon');
		        $span.removeClass('glyphicon-chevron-up').addClass('glyphicon-chevron-down');
		    })
		  ;
			$('#frm_config').validator().on('submit', function (e) {
				// everything looks good!
				if (!e.isDefaultPrevented()) {
					e.preventDefault();
					console.log("Form Submit");

					$.post('/config_form.json', $("#frm_config").serialize())
						.done( function(msg, textStatus, xhr) { 
  						Notify(2, 'ok', 'success', 'Enregistrement effectué', xhr.status+' '+msg);
						})
						.fail( function(xhr, textStatus, errorThrown) {
  						Notify(4, 'remove', 'danger', 'Erreur lors de l\'enregistrement', xhr.status+' '+errorThrown);
 						}
					);
				}
			});
 			$('#file_toload').change(function() {
				var $txtf = $('#txt_upload_file');
				var $btnf = $('#btn_upload_file');
				var okf = true;
				var ff = this.files[0];
				var namef = ff.name.toLowerCase();
				var sizef = ff.size;
				var typef = ff.type;
				var htmlf = 'Fichier:' + namef + '&nbsp;&nbsp;type:' + typef + '&nbsp;&nbsp;taille:' + sizef + ' octets'
				console.log('name='+namef);
				console.log('size'+sizef);
				console.log('type='+typef);
				$('#pgfile').removeClass('show').addClass('hide');
				$('#sfile').text( namef + ' : '); 
				if (okf) {
					$btnf.removeClass('hide');
					label = 'Transférer vers filesystem';
					$btnf.val(label);
					$('#file_info').html('<strong>' + label + '</strong> ' + html);
				} else {
					$txtf.attr('readonly', '');
					$txtf.val('');
					$txtf.attr('readonly', 'readonly');
					$btnf.addClass('hide');
				}
				
				return okf;
			});
			$('#file_fw').change(function() {
				var $txt = $('#txt_upload_fw');
				var $btn = $('#btn_upload_fw');
				var ok = true;
				var f = this.files[0];
				var name = f.name.toLowerCase();
				var size = f.size;
				var type = f.type;
				var html = 'Fichier:' + name + '&nbsp;&nbsp;type:' + type + '&nbsp;&nbsp;taille:' + size + ' octets'
				console.log('name='+name);
				console.log('size'+size);
				console.log('type='+type);
				$('#pgfw').removeClass('show').addClass('hide');
				$('#sfw').text( name + ' : ');  
				if (!f.type.match('application/octet-stream')) {
					Notify(3, 'remove', 'danger', 'Type de fichier non conforme', 'Le fichier de mise à jour doit être un fichier binaire');
					ok = false;
				} else if (name!="traitement_tempo_vmc.ino.bin" && name!="traitement_tempo_vmc.ino.spiffs.bin") {
					Notify(5, 'remove', 'danger', 'Nom de fichier incorrect', 'Le fichier de mise à jour doit être nommé <ul><li>_vmc.ino.bin (Micro-logiciel) ou</li><li>traitement_tempo_vmc.ino.spiffs.bin (Système de fichiers)</li></ul>');
					ok = false;
				}
				if (ok) {
					$btn.removeClass('hide');
					if ( name==="traitement_tempo_vmc.ino.bin") {
						label = 'Mise à jour Micro-Logiciel ou spiffs';
					}	else {
						label = 'Mise à jour SPIFFS';
					}
					$btn.val(label);
					$('#fw_info').html('<strong>' + label + '</strong> ' + html);
				} else {
					$txt.attr('readonly', '');
					$txt.val('');
					$txt.attr('readonly', 'readonly');
					$btn.addClass('hide');
				}
				return ok;
			});
			$('#btn_upload_file').click(function() {
		    var formData = new FormData($('#frm_file')[0]);
		    $.ajax({
	        url: '/uploadfile',  
	        type: 'POST',
	        data: formData,
	        cache: false,
	        contentType: false,
	        processData: false,
	        xhrf: function() {  
		        var myXhrf = $.ajaxSettings.xhrf();
		        if(myXhrf.upload)
		          myXhrf.upload.addEventListener('progress',progressUploadf, false); 
		        return myXhrf;
				},
				beforeSendf: function () { 
 						$('#pgfile').removeClass('hide');
	        		},
					success: function(msg, textStatus, xhrf) {	
						Notify(2, 'floppy-saved', 'success','Transfert du fichier terminé', '<strong>'+xhrf.status+'</strong> '+msg); 
					},
					error: function(xhrf, textStatus, errorThrown) {
 						$('#pfile').removeClass('progress-bar-success').addClass('progress-bar-danger');
						Notify(4, 'floppy-remove', 'danger', 'Erreur lors du transfert du fichier '+name,'<strong>'+xhrf.status+'</strong> '+errorThrown);
					}
				});
			});
			$('#btn_upload_fw').click(function() {
		    var formData = new FormData($('#frm_fw')[0]);
		    $.ajax({
	        url: '/update',  
	        type: 'POST',
	        data: formData,
	        cache: false,
	        contentType: false,
	        processData: false,
	        xhr: function() {  
            var myXhr = $.ajaxSettings.xhr();
            if(myXhr.upload)
              myXhr.upload.addEventListener('progress',progressUpload, false); 
            return myXhr;
	        },
	        beforeSend: function () { 
 				$('#pgfw').removeClass('hide');
	        },
				success: function(msg, textStatus, xhr) {	
					Notify(2, 'floppy-saved', 'success','Envoi de la mise à jour terminé', '<strong>'+xhr.status+'</strong> '+msg); 
					waitReboot();
				},
				error: function(xhr, textStatus, errorThrown) {
					$('#pfw').removeClass('progress-bar-success').addClass('progress-bar-danger');
					Notify(4, 'floppy-remove', 'danger', 'Erreur lors de la mise à jour du fichier '+name,'<strong>'+xhr.status+'</strong> '+errorThrown);
				}
		    });
			});
			window.onload = function() {  
				var url = document.location.toString();  
   			var full = location.protocol+'//'+location.hostname+(location.port ? ':'+location.port: '');
				console.log (url);
				console.log (full);
				console.log (location.port);
				if (url.match('#')) {  
					$('.nav-tabs a[href=#' + url.split('#')[1] + ']').tab('show');  
				}  
				$('.nav-tabs a[href=#' + url.split('#')[1] + ']').on('shown', function(e) {  
					window.location.hash = e.target.hash;  
                });
				$('#tab_sys_data').bootstrapTable('refresh',{silent:true, url:'/system.json'});
 			}  
			$('#btn_test').click(function(){ waitReboot(); });
// Détection
if(typeof localStorage!='undefined') {
  // Récupération de la valeur dans web storage
  var nbvisites = localStorage.getItem('visites');
  // Vérification de la présence du compteur
  if(nbvisites!=null) {
    // Si oui, on convertit en nombre entier la chaîne de texte qui fut stockée
    nbvisites = parseInt(nbvisites);
  } else {
    nbvisites = 1;
  }
  console.log("nbvisites= " + nbvisites);
  // Incrémentation
  nbvisites++;
  // Stockage à nouveau en attendant la prochaine visite...
  localStorage.setItem('visites',nbvisites);
  // Affichage dans la page
  //document.getElementById('visites').innerHTML = nbvisites;
} else {
  alert("localStorage n'est pas supporté");
}
