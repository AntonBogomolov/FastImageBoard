var currZIndex = 999;
var mouseX = 0;
var mouseY = 0;
var showWaitIco = false;

function answerClick(block, insertWhat, threadId)
{		
		var blockId = "#" + block;
		
		$("input[name='thread_id']").val(threadId);
		$("input[name='parent_id']").val(block);		
		$(insertWhat).insertAfter(blockId);
		
		$(".new_post_form").show();
		$(window).scrollTop($('.new_post_form').offset().top - 100);
}

function submitClick(elem)
{
		$("input[type='submit']").prop("disabled", true);
		elem.submit();
}

function newThreadFormHide()
{
	var status = $('.new_thread_form').css('display');
	
	if(status == 'none')
	{
		$('#new_thread_form_status').text('[Скрыть]');
		$('.new_thread_form').css({"display": "block"});
	}
	else
	if(status == 'block')
	{
		$('#new_thread_form_status').text('[Показать]');
		$('.new_thread_form').css({"display": "none"});
	}	
}

function hideClick(blockId, isHidden)
{
		//$(blockId).css({"display": "none"});
		var comUrl = '?command=hide';
		var metod  = 'add';
		if (isHidden) metod  = 'remove';	
		var userId = $("input[name='user_id']").val();
		var postData = 'id=' + blockId +'&metod=' + metod + "&user_id=" + userId;		
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
    		showWaitIco = false;
  		},
		error: function () 
  		{
  			showWaitIco = false;
  		}
		});
}

function delClick(blockId)
{
		var comUrl = '?command=del';
		var userId = $("input[name='user_id']").val();
		var id = 'id=' + blockId + "&user_id=" + userId;		
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: id,
  		success: function(msg)
  		{  			
    		location.reload();
			showWaitIco = false;
  		},
  		error: function () 
  		{
  			showWaitIco = false;
  		}
		});
}

function addToFavorite(blockId, isHidden)
{
		var comUrl = '?command=favorite';
		var metod  = 'add';
		if (isHidden) metod  = 'remove';	
		var userId = $("input[name='user_id']").val();
		var postData = 'id=' + blockId +'&metod=' + metod + "&user_id=" + userId;		
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
			showWaitIco = false;
  		},
  		error: function () 
  		{
  			showWaitIco = false;
  		}
		});		
}

function fixThread(blockId, isHidden)
{
		var comUrl = '?command=fix';
		var metod  = 'add';
		if (isHidden) metod  = 'remove';	
		var userId = $("input[name='user_id']").val();
		var postData = 'id=' + blockId +'&metod=' + metod + "&user_id=" + userId;		
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
			showWaitIco = false;
  		},
  		error: function () 
  		{
  			showWaitIco = false;
  		}
		});		
}

function fixToAll(blockId, isHidden)
{
		var comUrl = '?command=fixtoall';
		var metod  = 'add';
		if (isHidden) metod  = 'remove';	
		var userId = $("input[name='user_id']").val();
		var postData = 'id=' + blockId +'&metod=' + metod + "&user_id=" + userId;		
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
			showWaitIco = false;
  		},
  		error: function () 
  		{
  			showWaitIco = false;
  		}
		});		
}

function allowPremod(blockId)
{
		var comUrl = '?command=allowpremod';	
		var userId = $("input[name='user_id']").val();
		var postData = 'id=' + blockId + "&user_id=" + userId;		
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
  		}
		});		
}

function complain(blockId)
{
		var comUrl = '?command=complain';	
		var metod  = 'add';		
		var userId = $("input[name='user_id']").val();
		var postData = 'id=' + blockId + "&metod=" + metod + "&user_id=" + userId;		
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  		    		
    		hideClick(blockId);
  		}
		});		
}

function clearComplain(blockId)
{
		var comUrl = '?command=complain';	
		var metod  = 'remove';		
		var userId = $("input[name='user_id']").val();
		var postData = 'id=' + blockId + "&metod=" + metod + "&user_id=" + userId;		
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
  		}
		});		
}

function banUser(userId, roTime, closeAccess)
{
		var comUrl = '?command=ban';	
		var metod  = 'add';			
		var closeAccessStr = closeAccess;			
		var mid = $("input[name='user_id']").val();
		var postData = 'id=' + userId + "&metod=" + metod + "&rotime=" + (roTime*60*60*24) + "&closeaccess=" + closeAccessStr + "&user_id=" + mid;	
						
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();    		
  		}
		});		
}

function unbanUser(userId)
{
		var comUrl = '?command=ban';	
		var metod  = 'remove';				
		var mid = $("input[name='user_id']").val();
		var postData = 'id=' + userId + "&metod=" + metod + "&user_id=" + mid;			
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
  		}
		});		
}

function createSurveyQuestions(questCnt)
{
		var surveyQuestionsBlock = $('#survey_questions');		
		if(questCnt > 30) questCnt = 30;
		
		$(surveyQuestionsBlock).empty();
				
		for(var i = 0 ; i < questCnt; i++)	
		{
			var textId = 'q' + i + 'text';		
			var fieldCntId = 'q' + i + 'fcnt';		
			var fieldsId = 'q' + i + 'fields';
			
			var typeValueGetter = "$('input:radio[name=qtype"+i+"]:checked').val()";
			var fCntValueGetter = "$('#"+fieldCntId+"').val()";
			
			var htmlContent = '<div class="survey_quest survey_question_title">';
			htmlContent += '<span> Текст вопроса №' + (i+1) + ':  </span>';
			htmlContent += '<input type="text" maxlength="60" size="60" id="'+textId+'">';
			htmlContent += '<br>';
			htmlContent += '<span> Тип вариантов ответа: </span>';
			htmlContent += '<input type="radio" name="qtype'+i+'" value="0" checked><span> Checkbox  </span>';
			htmlContent += '<input type="radio" name="qtype'+i+'" value="1"><span> Radiobutton  </span>';
			htmlContent += '<input type="radio" name="qtype'+i+'" value="3"><span> Combobox  </span>';
			htmlContent += '<br>';
			htmlContent += '<span> Количество вариантов ответа: </span>';
			htmlContent += '<input name="fcnt" value="4" type="text" maxlength="4" size="2" id="'+fieldCntId+'"><span>  </span>';
			htmlContent += '<input type="button" value="Применить" onclick="createSurveyField('+i+','+typeValueGetter+','+fCntValueGetter+')">';
			htmlContent += '<br>';
			htmlContent += '</div>';
			htmlContent += '<div id="'+fieldsId+'"></div>';						
			$(surveyQuestionsBlock).append(htmlContent);
		}
}

function createSurveyField(currQuestion, fType, fCnt)
{
		var surveyFieldsBlock = $('#q'+currQuestion+'fields');
		$(surveyFieldsBlock).empty();
		
		if(fCnt > 20) fCnt = 20;
		if(fType < 0 || fType > 3) return;
		
		for(var i = 0 ; i < fCnt; i++)	
		{
			var fieldId = 'q' + currQuestion + 'f' + i;	
			var textId = fieldId + 'text';
			
			var htmlContent = '<div class="survey_question_field">';
			htmlContent += '<span>Текст варианта ответа №'+(i+1)+' </span>';
			htmlContent += '<input type="text" maxlength="60" size="60" id="'+textId+'">';
			htmlContent += '</div>';
			$(surveyFieldsBlock).append(htmlContent);
		}
}

function createSurvey(daysCnt)
{
		var surveyQuestionsBlock = $('#survey_questions');	
		
		var surveyTitle = $('#surveyTitle').val();
		surveyTitle = replaceAll(surveyTitle, ";", " ");
		
		var surveyDescription = $('#surveyDescription').val();
		surveyDescription = replaceAll(surveyDescription, ";", " ");
		
		var surveyQuestionsCnt = $("#questCnt").val();				
		if(daysCnt > 30) daysCnt = 30;
		if(daysCnt <= 0) daysCnt = 1;
		var surveyLifeTime = daysCnt * 60 * 60 * 24;
		
		if(surveyQuestionsCnt > 30) surveyQuestionsCnt = 30;
		
		var contentStr = '';
		var questionsStr = '';
		var correctQuestionsCnt = 0;
		for(var i = 0; i < surveyQuestionsCnt; i++)
		{			
			var currQuestionText = $('#q'+i+'text').val();
			currQuestionText = replaceAll(currQuestionText, ";", " ");
			var currQuestionType = $('input:radio[name=qtype'+i+']:checked').val();
			if(currQuestionType < 0 || currQuestionType > 4) continue;
			var currQuestionFieldsCnt = $('#q' + i + 'fcnt').val();
			if(currQuestionFieldsCnt > 20) currQuestionFieldsCnt = 20;
			if(currQuestionFieldsCnt <= 0) continue;			
			
			var currQuestionStr = '';	
			var questPartsCnt = 0;		
			if(currQuestionType == 0 || currQuestionType == 1)
			{
				currQuestionStr += (2+4*currQuestionFieldsCnt) + ';' + currQuestionText + ';' + currQuestionFieldsCnt + ';';
				for(var j = 0; j < currQuestionFieldsCnt; j++)
				{
					var currFieldText = $('#q'+i+'f'+j+'text').val();
					currFieldText = replaceAll(currFieldText, ";", " ");						
					currQuestionStr += '3;' + currQuestionType + ';' + currFieldText + ';' + '0;';
				}
			}else 
			if(currQuestionType == 3)
			{
				currQuestionStr += (6+currQuestionFieldsCnt*1)+';'+currQuestionText+';1;'+(3+currQuestionFieldsCnt*1)+';3;combo;'+currQuestionFieldsCnt+';';
				for(var j = 0; j < currQuestionFieldsCnt; j++)
				{
					var currFieldText = $('#q'+i+'f'+j+'text').val();
					currFieldText = replaceAll(currFieldText, ";", " ");						
					currQuestionStr += currFieldText + ';';
				}
			}
			else continue;
			
			questionsStr += currQuestionStr;
			correctQuestionsCnt++;
		}
		
		contentStr += correctQuestionsCnt + ';' + questionsStr;
		
		var comUrl = '?command=createsurvey';
		var mid = $('input[name="user_id"]').val();
		var postData = 'title=' + surveyTitle + "&description=" + surveyDescription + "&content=" + contentStr + "&lifetime=" + surveyLifeTime + "&user_id=" + mid;	
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
			showWaitIco = false;
  		},
		error: function () 
  		{
  			showWaitIco = false;
  		}
		});
}

function delSurvey(surveyId, userId)
{
		var comUrl = '?command=delsurvey';
		var mid = userId;
		var postData = "survey_id=" + surveyId + "&user_id=" + mid;	
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
    		showWaitIco = false;
  		},
		error: function () 
  		{
  			showWaitIco = false;
  		}
		});
}

function endSurvey(surveyId, userId)
{
		var comUrl = '?command=endsurvey';
		var mid = userId;
		var postData = "survey_id=" + surveyId + "&user_id=" + mid;	
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
    		showWaitIco = false;
  		},
		error: function () 
  		{
  			showWaitIco = false;
  		}
		});
}

function sendAnswerToSurvey(surveyId, userId)
{
		var comUrl = '?command=answertosurvey';	
		var mid = $('input[name="user_id"]').val();
		
		var surveyAnswerContent  = '';
		var surveyElemId = '#s' + surveyId;	
		var surveyQuestionsBlock = $(surveyElemId + ' .survey_questions');		
		
		var questionsCnt = $(surveyQuestionsBlock).children().length;
		surveyAnswerContent += questionsCnt + ';';
		for (var i = 0; i < questionsCnt; i++)
		{
			var currQuestionId = 's' + surveyId + 'q' + i;
			var currQuestion = $('#' + currQuestionId);
			
			var fieldsCnt = $(currQuestion).children().not('.survey_question_title').not('span').not('br').length;
			
			surveyAnswerContent += fieldsCnt + ';';
			for (var j = 0; j < fieldsCnt; j++)
			{
				var currFieldId = 's' + surveyId + 'q' + i + 'f' + j;
				var currField  = $('#' + currFieldId);
				var inputType  = $(currField).attr('type');
				
				if (inputType == 'checkbox')
				{
					surveyAnswerContent += '0;';
					if($(currField).prop('checked')) surveyAnswerContent += '1;';
					else surveyAnswerContent += '0;';
				}
				if (inputType == 'radio')
				{
					surveyAnswerContent += '1;';
					if($(currField).prop('checked')) surveyAnswerContent += '1;';
					else surveyAnswerContent += '0;';
				}
				if (inputType == 'text')
				{
					surveyAnswerContent += '2;';
					var textVal = $(currField).val();					
					if (textVal.indexOf(';') != -1) textVal = ' ';					
					surveyAnswerContent += textVal + ';';
				}
				if ($(currField).is('select'))
				{
					surveyAnswerContent += '3;';
					surveyAnswerContent += $(currField).val() + ';';
				}
			}
		}
		
		var postData = 'id=' + surveyId + "&content=" + surveyAnswerContent + "&user_id=" + mid;			
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
			showWaitIco = false;
  		},
		error: function () 
  		{
  				showWaitIco = false;
  		}
		});	
}

function linkToMessageClick(link)
{
		var comUrl = '?command=getmessage';	
		var userId = $("input[name='user_id']").val();	
		var messageId = $(link).text();
		var returnPage = $("input[name='return_page']").val();
		var postData = 'id=' + messageId + "&user_id=" + userId + "&retpage" + returnPage;		
		
		if($('div').is('#popup'+messageId))
		{
			 removeElem($('#popup'+messageId));
			 return;
		}
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{
  			if(msg == "thread not exist" || msg == "post not exist") return;
  			
  			var htmlData = msg;
  			var newPopupBlock = '<div class="popup_block" id="popup'+messageId+'">'+htmlData+'</div>';
  			if($('div').is('#popup'+messageId)) return;
  			  			
  			createRemoveAllPopusButton($(link).offset().left - 65,$(link).offset().top + 20);
  			
  			$(newPopupBlock).insertAfter('.content_end');
  			var newBlock = $('#popup'+messageId);  			
  			$(newBlock).offset({top: $(link).offset().top + 50 + Math.random()*50, left: $(link).offset().left + 10 + Math.random()*50});
  			$(newBlock).find('.post').toggleClass('popup_decore');
			$(newBlock).find('.thread').toggleClass('popup_decore');	
			$(newBlock).css({'z-index': currZIndex});	
			currZIndex++;	
			showWaitIco = false;
			
			parseAllMessages();			
  		},
		error: function () 
  		{
  				showWaitIco = false;
  		}
		});		
}

function createRemoveAllPopusButton(x,y)
{
	var buttonHtml = '<div class="post popup_decore removeAllPopusButton">' + 'закрыть все окна' + '</div>';
	if($('div').is('.removeAllPopusButton'))
	{
		$('.removeAllPopusButton').offset({top: y, left: x});
		return;
	} 
	
	$(buttonHtml).insertAfter('.content_end');
	var buttonElem = $('.removeAllPopusButton');
	buttonElem.width(130);
	buttonElem.height(20);
	buttonElem.css({'background-color': '#eeaa88'});
	buttonElem.css({'color': '#600000'});
	buttonElem.css({'min-width': '80px'});
	buttonElem.css({'min-height': '20px'});
	buttonElem.css({'z-index': '9999'});
	buttonElem.offset({top: y, left: x});
	buttonElem.click(function () {removeAllPopups()});
}

function removeElem(elem)
{
	$(elem).remove();
}

function removeAllPopups()
{
	$('.popup_decore').remove();
}

function expandThreadClick(threadId)
{	
		var filter = '#t' + threadId + ' div.thread_posts_container';
		var userId = $("input[name='user_id']").val();		
		var comUrl = '?command=getthreadposts';
		var id = 'id=' + threadId + "&user_id=" + userId;	
		showWaitIco = true;	
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: id,
  		success: function(msg)
  		{  			
  			$('#new_post_form').insertAfter('.new_thread_form');
			$('#new_post_form').css({"display": "none"});
			showWaitIco = false;
		
			$(filter).empty();
    		$(filter).append(msg);
    		
    		parseAllMessages();    		
  		},
  		error: function () 
  		{
  				showWaitIco = false;
  		}
		});
}

function saveUserAgeRating(selectName)
{
		var comUrl = '?command=setuseragerating';
		var ageRating = parseInt($("select[name='"+selectName+"'] :selected").val());
		var userId = $("input[name='user_id']").val();				 
		var postData = 'agerating=' + ageRating + "&user_id=" + userId;		
		showWaitIco = true;
		
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  			
    		location.reload();
			showWaitIco = false;
  		},
  		error: function () 
  		{
  				showWaitIco = false;
  		}
		});
}

function loadSettings()
{
		var comUrl = '?command=login';		
		var userId = $("input[name='user_id']").val();
		var postData = 'userKey=' + $("input[name='new_userkey']").val() + "&user_id=" + userId;		
		showWaitIco = true;
				
		$.ajax({
		type: 'POST',
  		url: comUrl,
  		data: postData,
  		success: function(msg)
  		{  	
  			//alert(msg);		
  			//if (!msg == 'Error')
  			//{  				
    			location.reload();
				showWaitIco = false;
    		//}
  		},
		error: function () 
  		{
  				showWaitIco = false;
  		}
		});		
}

function insertBlockAfter(newBlock, prevBlock) 
{
	$(newBlock).insertAfter(prevBlock);
}

function gotoPage(pageURI)
{
		location.href = pageURI;
}

$(window).scroll(function() 
{
     if  ($(window).scrollTop() + $(window).height() > $(document).height() - 10) 
     {
         var isThreadPage = $("input").is("#thCnt");              
         if(isThreadPage)
         {
         	var currThreadCnt = parseInt($("input[name='curr_threads_cnt']").val());
				var allThreadCnt  = parseInt($("input[name='all_threads_cnt']").val());
				var catId 			= $("input[name='category_id']").val();
				var minThreadPos  = $("input[name='min_thread_pos']").val();
				var returnPage 	= $("input[name='return_page']").val();
				var threadsToLoadCnt = 1;
				
				currThreadCnt = $('.thread').length;														
				if (currThreadCnt >= allThreadCnt) return;
				minThreadPos = parseInt($("input[name='currThreadPos']:last").val());				
				threadsToLoadCnt = 1;
				
				var ids = "";
				$('.thread').each(function(indx, element){	
					ids += element.id;
					ids += ";";
				});	
				
				showWaitIco = true;								
				var comUrl = '?command=getthreads';
				var userId = $("input[name='user_id']").val();
				var rData = 'lastthread=' + minThreadPos + '&catid=' + catId + '&thcnt=' + threadsToLoadCnt + '&retpage=' + returnPage + "&exsistids=" + ids + "&user_id=" + userId;		
						
				$.ajax({
				type: 'POST',
  				url: comUrl,
  				data: rData,  				
  				success: function(msg)
  				{  			
  					$(msg).insertAfter('.thread:last');
					  					
					parseAllMessages();  						
					
  					currThreadCnt += threadsToLoadCnt; 
  					$("input[name='min_thread_pos']").val(parseInt($("input[name='currThreadPos']:last").val()));  					
  					$("input[name='curr_threads_cnt']").val($('.thread').length);
					showWaitIco = false;	
  				},
  				error: function () 
  				{
  					showWaitIco = false;
  				}
  				
				});
         }    
     }
});

function checkFiles(element, isThreadFiles)
{
	var elementsNamePrefix = "thread"; 
	if(!isThreadFiles) elementsNamePrefix = "post";
	var maxFileSize = 10;
	var extentionsArray = ["jpg", "jpeg", "gif", "png", "zip", "rar", "mp3", "wav", "mpeg","flac", "mpg","mp4","avi", "webm", "flv", "txt", "doc", "xls", "waw", "ogg", "wma", "wmv", "cpp", "h", "pdf", "djw"];
	var filesCnt  		= parseInt($("input[name='"+elementsNamePrefix+"_files_cnt']").val());	
	var currFileNum 	= parseInt(element.id.slice(-1));
	var statusElementId = "#"+elementsNamePrefix+"FileStatus" + currFileNum; 

   var val = $(element).val();
	var ext = val.substring(val.lastIndexOf('.') + 1).toLowerCase();
	if ($.inArray(ext, extentionsArray) == -1)
	{
		$(statusElementId).text("Тип данного файла не подходит");
		$(element).val("");
		return false;
	}

	var fileSize = ($(element)[0].files[0].size / 1024 / 1024); 
	if (fileSize > maxFileSize)
   {
		$(statusElementId).text("Файл должен быть меньше " + maxFileSize + "Мб");
		$(element).val("");
		return false;
   }
   
   $(statusElementId).text("Готово");
   if(filesCnt == currFileNum && filesCnt < 9)
   {
   	filesCnt++;
   	$("input[name='"+elementsNamePrefix+"_files_cnt']").val(filesCnt);
   	createFileInput(filesCnt, isThreadFiles);
	}
}

function createFileInput(currFilesCnt, isThreadFiles)
{
	var elementsNamePrefix = "thread"; 
	if(!isThreadFiles) elementsNamePrefix = "post";
	
	var controlElementId 	= elementsNamePrefix+"FileDel" 	 + currFilesCnt; 
	var statusElementId 		= elementsNamePrefix+"FileStatus" + currFilesCnt; 
	var fileElementId   		= elementsNamePrefix+"FileUpload" + currFilesCnt; 	
	var fileElementName		= elementsNamePrefix+"file" 		 + currFilesCnt; 	
	var prevFileElementId   = ".new_"+elementsNamePrefix+"_form .file_separator:last"; 
	if(currFilesCnt == 0) prevFileElementId = "#"+elementsNamePrefix+"_files_cnt";
	
	var controlElemHtml = "<div class='file_control control_text'><span id='"+controlElementId+"' onclick='delFile(this, "+isThreadFiles+")'>[Очисить] </span></div>";	
	$(controlElemHtml).insertAfter(prevFileElementId);
	var inputElemHtml = 	"<div class='file_input'><input id='"+fileElementId+"' type='file' name='"+fileElementName+"' onchange='checkFiles(this, "+isThreadFiles+")' /></div>"
	$(inputElemHtml).insertAfter(".new_"+elementsNamePrefix+"_form .file_control:last");	
	var statusElemHtml = "<div class='file_status control_text'><span id="+statusElementId+"></span></div>";	
	$(statusElemHtml).insertAfter(".new_"+elementsNamePrefix+"_form .file_input:last");
	var separatorElemHtml = "<div class='file_separator'></div>";	
	$(separatorElemHtml).insertAfter(".new_"+elementsNamePrefix+"_form .file_status:last");

}

function delAllFiles(isThreadFiles)
{
	var elementsNamePrefix = "thread"; 
	if(!isThreadFiles) elementsNamePrefix = "post";
	
	var postFormClass = ".new_"+elementsNamePrefix+"_form ";
		
	$(postFormClass + ".file_control").remove();
	$(postFormClass + ".file_input").remove();
	$(postFormClass + ".file_status").remove();	
	
	$("input[name='"+elementsNamePrefix+"_files_cnt']").val(0);
	
	createFileInput(0,isThreadFiles);
}

function delFile(elem, isThreadFiles)
{
	var elementsNamePrefix = "thread"; 
	if(!isThreadFiles) elementsNamePrefix = "post";
	
	var currFileNum = parseInt(elem.id.slice(-1));
	$("input[name='"+elementsNamePrefix+"file"+currFileNum+"']").val("");
	$("#"+elementsNamePrefix+"FileStatus"+currFileNum).text("");
}

function fileClick(elem, filePath, filePrevPath, fileType, imgWidth, imgHeight, imgPrevWidth, imgPrevHeight)
{
	if(fileType == "image") 
	{
		if(imgWidth == 0 || imgHeight == 0 || imgPrevWidth == 0 || imgPrevHeight == 0 ) return;
		if(elem.width != imgPrevWidth && elem.height != imgPrevHeight)
		{
			elem.width  = imgPrevWidth;	
			elem.height = imgPrevHeight;	
			elem.src = filePrevPath;
		}
		else 
		{
			var clientWidth = document.documentElement.clientWidth * 0.92 - $(elem).offset().left;	
			if (clientWidth < imgPrevWidth) clientWidth = imgPrevWidth;
			
			if(imgWidth > clientWidth)
			{				
				var coeff =  (clientWidth) / imgWidth;
				imgWidth  = imgWidth  * coeff;
				imgHeight  = imgHeight  * coeff;
			}
			elem.width  = imgWidth;	
			elem.height = imgHeight;	
			elem.src = filePath;
		}
		
	}
	else 
	{
		gotoPage(filePath);
	}
}

////////////////////////////////////////////////////////////////

String.prototype.insert = function(index, string) 
{
  if (index > 0)
    return this.substring(0, index) + string + this.substring(index, this.length);
  else
    return string + this;
};

function escapeRegExp(string) 
{
    return string.replace(/([.*+?^=!:${}()|\[\]\/\\])/g, "\\$1");
}

function replaceAll(string, find, replace) 
{
  return string.replace(new RegExp(escapeRegExp(find), 'g'), replace);
}

function parseMessage(messageElem)
{	
	var resultMessage = "";
   var textHTML = $(messageElem).html();   

	var ST_INIT 	 = 0;
	var ST_ITAL_BEG = 1;
	var ST_BOLD_BEG = 3;
	var ST_SPOILER_BEG = 5;
	var ST_QUOTE_BEG= 7;
	var ST_LINK_BEG = 9;
	var ST_CODE_BEG = 10;
	var ST_REPLAY_BEG = 11;

	var maxDeep = 5;

	var states = new Array();
	var startPos = new Array();
	states.push(ST_INIT);
	startPos.push(0);
	
	var state = ST_INIT;
	var stPos = 0;
	for(var pos = 0; pos < textHTML.length; ++pos)
   {
   	if(states.length > 0)
   	{
			state = states[states.length - 1];
			stPos = startPos[startPos.length - 1];
		}
		else 
		{
			var state = ST_INIT;
			var stPos = 0;
		}
        switch(textHTML.charAt(pos))
        {
        	case 'b':
        		if(textHTML.length - 1 > pos && textHTML.charAt(pos + 1) == '(')
				{
					if(states.length < maxDeep)
					{
						states.push(ST_BOLD_BEG);
						startPos.push(resultMessage.length);
					}
					pos++;
				}
				else
				{
					resultMessage = resultMessage.concat("b");
				}
			break;
			case 'i':
				if(textHTML.length - 1 > pos && textHTML.charAt(pos + 1) == '(')
				{
					if(states.length < maxDeep)
					{
						states.push(ST_ITAL_BEG);
						startPos.push(resultMessage.length);
					}
					pos++;
				}
				else
				{
					resultMessage = resultMessage.concat("i");
				}
			break;
			case 'l':
				if(textHTML.length - 1 > pos && textHTML.charAt(pos + 1) == '(')
				{
					if(states.length < maxDeep)
					{
						states.push(ST_LINK_BEG);
						startPos.push(resultMessage.length);
					}
					pos++;
				}
				else
				{
					resultMessage = resultMessage.concat("l");
				}
			break;
			case 'r':
				if(textHTML.length - 1 > pos && textHTML.charAt(pos + 1) == '(')
				{
					if(states.length < maxDeep)
					{
						states.push(ST_REPLAY_BEG);
						startPos.push(resultMessage.length);
					}
					pos++;
				}
				else
				{
					resultMessage = resultMessage.concat("r");
				}
			break;
			case 's':
				if(textHTML.length - 1 > pos && textHTML.charAt(pos + 1) == '(')
				{
					if(states.length < maxDeep)
					{
						states.push(ST_SPOILER_BEG);
						startPos.push(resultMessage.length);
					}
					pos++;
				}
				else
				{
					resultMessage = resultMessage.concat("s");
				}
			break;
			case 'q':
				if(textHTML.length - 1 > pos && textHTML.charAt(pos + 1) == '(')
				{
					if(states.length < maxDeep)
					{
						states.push(ST_QUOTE_BEG);
						startPos.push(resultMessage.length);
					}
					pos++;
				}
				else
				{
					resultMessage = resultMessage.concat("q");
				}
			break;
			
			case ')':
				if(state == ST_BOLD_BEG)
				{
					resultMessage = resultMessage.insert(stPos, "<b>");
					resultMessage = resultMessage.concat("</b>");
					states.pop();
					startPos.pop();	
					continue;				
				}
				if(state == ST_ITAL_BEG)
				{
					resultMessage = resultMessage.insert(stPos, "<i>");
					resultMessage = resultMessage.concat("</i>");
					states.pop();
					startPos.pop();	
					continue;					
				}		
				if(state == ST_SPOILER_BEG)
				{
					resultMessage = resultMessage.insert(stPos, "<span class='spoiler'>");
					resultMessage = resultMessage.concat("</span>");					
					states.pop();
					startPos.pop();
					continue;	
				}
				if(state == ST_REPLAY_BEG)
				{
					resultMessage = resultMessage.insert(stPos, "&gt;&gt;<span class='replyToLink'>");
					resultMessage = resultMessage.concat("</span>");					
					states.pop();
					startPos.pop();
					continue;	
				}
				if(state == ST_LINK_BEG)
				{
					resultMessage = resultMessage.insert(stPos, "<a href='");
					resultMessage = resultMessage.concat("'>link</a>");					
					states.pop();
					startPos.pop();
					continue;	
				}
				if(state == ST_QUOTE_BEG)
				{
					resultMessage = resultMessage.insert(stPos, "&laquo;<span class='quote'>");
					resultMessage = resultMessage.concat("</span>&raquo;");					
					states.pop();
					startPos.pop();
					continue;	
				}
				resultMessage = resultMessage.concat(")");
			break;			
						
			default:
				resultMessage = resultMessage.concat(textHTML.charAt(pos));
			break;
        }
	}
	
	// YOUTUBE LINKS
	var startPos = 0;
	var endPos = 0;
	startPos = resultMessage.indexOf("https://www.youtube.com/watch?v=", startPos);
	if(startPos == -1) startPos = resultMessage.indexOf("http://www.youtube.com/watch?v=", startPos);
	endPos = resultMessage.indexOf("&nbsp", startPos);
	while (startPos != -1 && endPos != -1)
	{			
		resultMessage = resultMessage.insert(endPos, "' type='video/youtube'> </video></div>");
		resultMessage = resultMessage.insert(startPos, "<div class='youtube'><video width='360' height='240' controls> <source src='");
						
		startPos = resultMessage.indexOf("https://www.youtube.com/watch?v=", endPos + 70);
		if(startPos == -1) startPos = resultMessage.indexOf("http://www.youtube.com/watch?v=", endPos + 70);
		endPos = resultMessage.indexOf("&nbsp", startPos);
	}
	//
	
	resultMessage = replaceAll(resultMessage, "[div]", "<br>");
	resultMessage = replaceAll(resultMessage, "[/div]", "");

	//	REMOVE HTML CODE IN COMMENT
	startPos = 0;
	endPos = 0;
	startPos = resultMessage.indexOf("[", 0);
	endPos 	= resultMessage.indexOf("]", startPos);	
	while (startPos != -1 && endPos != -1 )
	{			
		resultMessage = resultMessage.substring(0, startPos).concat(resultMessage.substring(endPos+1, resultMessage.length));		
					
		startPos = resultMessage.indexOf("[", startPos-1);
		endPos = resultMessage.indexOf("]", startPos);		
	}
	//	

    $(messageElem).html(resultMessage);  
 }

function parseAllMessages()
{
	$(".user_text").each(function(indx){
  			parseMessage($(this));
	});
	
	$(".reply_to_link").each(function(indx){
			$(this).click(function(){
				linkToMessageClick($(this));
			})			
	});
	
	$(".spoiler").each(function(indx){
		$(this).click(function(){
			$(this).toggleClass('selected');
		})			
	})		
	
	$('video').videoExtend();				
}
	  
 //////////////////////////////////////////////////////////////////////////////////
 
 $.fn.center = function() 
 {
    this.css("position", "fixed");
    this.css("top", ($(window).height()/2 - this.height()/2) + "px");
    this.css("left", ($(window).width()/2 - this.width()/2) + "px");
    return this;
}
 
 $(document).ready(
	function() 
	{
		////////////////////////////////////////
		nicEditors.allTextAreas();		
		$(".new_post_form").hide();
		////////////////////////////////////////
		parseAllMessages();		
		////////////////////////////////////////
		$(".menu_panel").click(function(){
			removeAllPopups();			
		})
		////////////////////////////////////////
		window.onbeforeunload = function() {
  			$("input[name='curr_threads_cnt']").val('0');  	
  			zIndex = 999;		
		}	
		////////////////////////////////////////
		$('form').submit(function() 
    	{
        	$('.loader_box').show();
			$('.loader_box').center();
			$('.loader_overlay').show();
   	}) 		
		////////////////////////////////////////
		$("body").mousemove(function (event) {
			if (showWaitIco) 
			{
				mouseX = event.pageX + 12;
				mouseY = event.pageY - 12;
				$(".wait_ico").offset({top: mouseY, left: mouseX});
				$(".wait_ico").css({'display': 'block'});				
			}
			else
			{
				$(".wait_ico").css({'display': 'none'});
			}
		}
		);
	}
);