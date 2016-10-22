.global		_start

_start:
	b		ResetHandler
	b		UndefinedHandler
	b		SWIHandler
	b		PrefetchHandler
	b		AbortHandler
	b		.
	b		IRQHandler
	b		fIQHandler

UndefinedHandler:
	b		.
	
SWIHandler:
	b . 
	
PrefetchHandler:
	b .
	
AbortHandler:
	b .
